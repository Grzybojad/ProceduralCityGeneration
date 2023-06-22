#pragma once

#include "BuildingPlot.generated.h"

USTRUCT(BlueprintType)
struct FBuildingPlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Outline;

	FVector GetCenter() const;
};
