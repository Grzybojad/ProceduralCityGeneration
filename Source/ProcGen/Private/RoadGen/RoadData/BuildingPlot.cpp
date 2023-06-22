#include "RoadGen/RoadData/BuildingPlot.h"

#include "PCGUtils.h"

FVector FBuildingPlot::GetCenter() const
{
	return FPcgUtils::CalculateCentroidIgnoringZ(Outline);
}
