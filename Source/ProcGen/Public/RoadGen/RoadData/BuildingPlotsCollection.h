#pragma once
#include "BuildingPlot.h"
#include "BuildingPlotsCollection.generated.h"

USTRUCT(BlueprintType)
struct FBuildingPlotsCollection
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FBuildingPlot> BuildingPlots;

	FVector GetCenter() const;
};
