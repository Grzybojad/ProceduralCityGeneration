// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AdjustRoadToTerrain.generated.h"

/**
 * 
 */
UCLASS()
class PROCGEN_API UAdjustRoadToTerrain : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DefaultToSelf ="ActorToIgnore"))
	static void GetPositionOnTerrain(
		const UObject* WorldContextObject,
		const FVector InitialPosition, const FVector InitialTangent,
		const float RoadWidth, const AActor* ActorToIgnore,
		ECollisionChannel TraceChannel, FVector& OutPosition, FVector& OutTangent);

protected:
	static bool VerticalLineTrace(const UWorld* World, const FVector Position, const AActor* ActorToIgnore, ECollisionChannel TraceChannel, FHitResult&
	                              Hit);
};
