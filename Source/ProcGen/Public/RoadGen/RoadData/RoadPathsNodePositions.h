#pragma once

#include "FRoadPathNodePositions.h"
#include "RoadPathsNodePositions.generated.h"

USTRUCT(BlueprintType)
struct FRoadPathsNodePositions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FRoadPathNodePositions> Paths;
};


