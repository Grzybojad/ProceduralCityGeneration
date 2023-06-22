// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadGen/SplineRoadsGenerator.h"

#include "Components/SplineComponent.h"
#include "RoadGen/AdjustRoadToTerrain.h"

USplineRoadsGenerator::USplineRoadsGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USplineRoadsGenerator::GenerateRoads(const FRoadPathsNodePositions RoadPathsNodePositions)
{
	DestroyedSpawned();
	
	for (const FRoadPathNodePositions RoadPath : RoadPathsNodePositions.Paths)
	{
		USplineComponent* NewSplineComponent = AddSplineComponent();
		NewSplineComponent->SetSplinePoints(RoadPath.Path, ESplineCoordinateSpace::Local, true);
		
		GenerateRoadMesh(NewSplineComponent);
	}
}

void USplineRoadsGenerator::DestroyedSpawned()
{
	for (auto& SplineComponentPtr : SplineComponents)
	{
		if (SplineComponentPtr.IsValid())
		{
			USplineComponent* SplineComponent = SplineComponentPtr.Get();
			SplineComponent->DestroyComponent();
		}
	}
	SplineComponents.Empty();
	
	for (auto& SplineMeshComponentPtr : SplineMeshComponents)
	{
		if (SplineMeshComponentPtr.IsValid())
		{
			USplineMeshComponent* SplineMeshComponent = SplineMeshComponentPtr.Get();
			SplineMeshComponent->DestroyComponent();
		}
	}
	SplineMeshComponents.Empty();
}

USplineComponent* USplineRoadsGenerator::AddSplineComponent()
{
	USplineComponent* NewSplineComponent = NewObject<USplineComponent>(this);
	NewSplineComponent->OnComponentCreated();
	NewSplineComponent->RegisterComponent();
	if (NewSplineComponent->bWantsInitializeComponent) NewSplineComponent->InitializeComponent();

	SplineComponents.Add(NewSplineComponent);
	
	return NewSplineComponent;
}

USplineMeshComponent* USplineRoadsGenerator::AddSplineMeshComponent()
{
	USplineMeshComponent* NewSplineMeshComponent = NewObject<USplineMeshComponent>(this);
	NewSplineMeshComponent->OnComponentCreated();
	NewSplineMeshComponent->RegisterComponent();
	if (NewSplineMeshComponent->bWantsInitializeComponent) NewSplineMeshComponent->InitializeComponent();

	NewSplineMeshComponent->SetStaticMesh(RoadMesh);
	NewSplineMeshComponent->SetMaterial(0, RoadMaterial);
	
	SplineMeshComponents.Add(NewSplineMeshComponent);
	
	return NewSplineMeshComponent;
}

void USplineRoadsGenerator::GenerateRoadMesh(const USplineComponent* Spline)
{
	const int SplinePoints = Spline->GetNumberOfSplinePoints();

	for (int i=0; i<SplinePoints-1; ++i)
	{
		USplineMeshComponent* NewSplineMeshComponent = AddSplineMeshComponent();

		FVector StartPosition, StartTangent;
		FVector EndPosition, EndTangent;
		Spline->GetLocalLocationAndTangentAtSplinePoint(i, StartPosition, StartTangent);
		Spline->GetLocalLocationAndTangentAtSplinePoint(i+1, EndPosition, EndTangent);

		AdjustSplineToTerrain(StartPosition, StartTangent, EndPosition, EndTangent);

		NewSplineMeshComponent->SetStartAndEnd(StartPosition, StartTangent, EndPosition, EndTangent, true);
		NewSplineMeshComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		NewSplineMeshComponent->UpdateMesh();
	}
}

void USplineRoadsGenerator::AdjustSplineToTerrain(FVector& StartPosition, FVector& StartTangent, FVector& EndPosition,
	FVector& EndTangent) const
{
	AdjustRoadPointToTerrain(StartPosition, StartTangent);
	AdjustRoadPointToTerrain(EndPosition, EndTangent);
	StartPosition = SubtractActorPosition(StartPosition);
	EndPosition = SubtractActorPosition(EndPosition);
}

void USplineRoadsGenerator::AdjustRoadPointToTerrain(FVector& Position, FVector& Tangent) const
{
	const FVector InputPosition = Position;
	const FVector InputTangent = Tangent;
	UAdjustRoadToTerrain::GetPositionOnTerrain(GetWorld(),
		InputPosition, InputTangent, RoadMeshWidth, GetOwner(), ECC_WorldStatic,
		Position, Tangent);
}

FVector USplineRoadsGenerator::SubtractActorPosition(const FVector& Position) const
{
	return Position - GetOwner()->GetActorLocation();
}

