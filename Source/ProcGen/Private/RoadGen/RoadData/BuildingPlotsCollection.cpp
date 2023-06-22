#include "RoadGen/RoadData/BuildingPlotsCollection.h"

#include "PCGUtils.h"

FVector FBuildingPlotsCollection::GetCenter() const
{
	TArray<FVector> Centers;
	for (const FBuildingPlot BuildingPlot : BuildingPlots)
	{
		Centers.Add(BuildingPlot.GetCenter());
	}
	return FPcgUtils::CalculateCentroidIgnoringZ(Centers);
}
