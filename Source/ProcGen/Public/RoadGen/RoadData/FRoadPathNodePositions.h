#pragma once

#include "FRoadPathNodePositions.generated.h"

USTRUCT(BlueprintType)
struct FRoadPathNodePositions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Path;
};
