 // Fill out your copyright notice in the Description page of Project Settings.


#include "RoadGen/AdjustRoadToTerrain.h"

void UAdjustRoadToTerrain::GetPositionOnTerrain(const UObject* WorldContextObject,
	const FVector InitialPosition, const FVector InitialTangent,
	const float RoadWidth,
	const AActor* ActorToIgnore, ECollisionChannel TraceChannel,
	FVector& OutPosition, FVector& OutTangent)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	const FVector UpVector = FVector(0, 0, 1);

	FVector SideVector = InitialTangent.RotateAngleAxis(90, UpVector).GetSafeNormal();
	FVector LeftTracePosition = InitialPosition + -(SideVector * RoadWidth * 0.5f);
	FVector RightTracePosition = InitialPosition + (SideVector * RoadWidth * 0.5f);

	FHitResult Hit, Hit2;
	bool DidHit1 = VerticalLineTrace(World, LeftTracePosition, ActorToIgnore, TraceChannel, Hit);
	bool DidHit2 = VerticalLineTrace(World, RightTracePosition, ActorToIgnore, TraceChannel, Hit2);
	
	

	// The tangent calculation is scuffed
	// FVector TerrainNormal = (Hit.Location + Hit.Normal).GetSafeNormal();
	// FVector TerrainDifference = TerrainNormal - FVector(0, 0, 1);
	// OutTangent = (InitialTangent + TerrainDifference).GetSafeNormal();
	OutTangent = InitialTangent;

	// DrawDebugLine(World, Hit.Location, Hit.Location + (InitialTangent.GetSafeNormal()*500), FColor::Red, true, -1);
	// DrawDebugLine(World, Hit.Location, Hit.Location + (OutTangent*500), FColor::Green, true, -1);

	if(DidHit1 && DidHit2)
	{
		OutPosition = (Hit.Location + Hit2.Location) / 2;
		OutPosition.Z = FMath::Max(Hit.Location.Z, Hit2.Location.Z);
	}
	else
	{
		OutPosition = InitialPosition;
	}
}

bool UAdjustRoadToTerrain::VerticalLineTrace(const UWorld* World, const FVector Position,
	const AActor* ActorToIgnore, const ECollisionChannel TraceChannel, FHitResult& Hit)
{
	FCollisionQueryParams QueryParams;

	constexpr float ZUpOffset = 20000;
	const FVector TraceStart = Position + FVector(0, 0, ZUpOffset);
	const FVector TraceEnd = TraceStart + FVector(0, 0, -(ZUpOffset*2));

	QueryParams.AddIgnoredActor(ActorToIgnore);
	const bool DidHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannel, QueryParams);

	// DrawDebugLine(World, TraceStart, TraceEnd, FColor::Magenta, true, -1);

	return DidHit;
}

