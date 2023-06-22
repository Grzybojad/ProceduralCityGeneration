#pragma once
#include "Math/UnrealMathUtility.h" // For PI
#include "RoadData/RoadSegment.h"
#include "RoadData/RoadSegmentToPlace.h"
#include "RoadData/RoadNetworkData.h"
#include "PCGUtils.h"

class FLocalConstraints
{
public:
	FLocalConstraints(const float MinNodeDistance, const float MinNodeToSegmentDistance,
					  const float NodeSnapDistance, const float MinConnectionAngle, const int MaxNodeConnections,
					  FRoadNetworkData* Network)
		: MinNodeDistance(MinNodeDistance),
		  MinNodeToSegmentDistance(MinNodeToSegmentDistance),
		  NodeSnapDistance(NodeSnapDistance),
		  MinConnectionAngle(MinConnectionAngle),
		  MaxNodeConnections(MaxNodeConnections),
		  Network(Network) {}

	bool HandleLocalConstraints(FRoadSegmentToPlace* SegmentToPlace, const bool IgnoreLimits = false) const;

	bool IsIntersectingSegments(const FRoadSegmentToPlace* SegmentToPlace, TArray<URoadSegment*>& IntersectedSegments) const;

	static URoadSegment* GetClosest(const FVector2D StartPos, const TArray<URoadSegment*>& IntersectedSegments);
	
protected:
	float MinNodeDistance;
	float MinNodeToSegmentDistance;
	float NodeSnapDistance;
	float MinConnectionAngle;
	int MaxNodeConnections;

	FRoadNetworkData* Network;

	
	bool HandleIntersections(FRoadSegmentToPlace* SegmentToPlace, const TArray<URoadSegment*>& IntersectedSegments, const bool IgnoreLimits = false) const;
	
	bool HandleNotIntersectingLine(FRoadSegmentToPlace* SegmentToPlace) const;

	bool SnapToNode(FRoadSegmentToPlace* SegmentToPlace, FRoadNode* NodeToSnapTo) const;
	
	bool ExtendToReachSegment(FRoadSegmentToPlace* SegmentToPlace, URoadSegment* SegmentToReach) const;
	
	void SplitSegment(URoadSegment* SegmentBeingSplit, FRoadSegmentToPlace* SplittingSegment) const;

	bool FinalCheck(const FRoadSegmentToPlace* SegmentToPlace) const;
};
