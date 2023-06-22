#pragma once

#include "RoadNode.h"
#include "RoadPath.h"
#include "RoadSegment.h"

enum ELocalConstraintsState
{
	Unmodified,
	ConnectedToExistingNode
};

class FRoadSegmentToPlace
{
	
public:
	FRoadSegmentToPlace();
	FRoadSegmentToPlace(const int32 Priority, FRoadNode* Start, const FVector2d& Target, FRoadPath* Path);

	int32 Priority;
	FRoadNode* Start;
	FVector2d Target;
	FRoadPath* Path;

	ELocalConstraintsState ConstraintsState;
	
	FRoadNode* NodeToConnectTo;
	URoadSegment* IntersectingSegment;
	
	bool operator<(const FRoadSegmentToPlace Other) const;

	void SetRandomTargetInDistance(const float Distance, const FRandomStream RandomStream);
	bool SetTargetToContinuePath(const float Distance, const FVector2D PreviousDirection, const float MaxTurnAngle, const FRandomStream RandomStream);

	float GetLength() const;
	FVector2D GetDirection() const;

	bool IsConnectedToNode(const FRoadNode* Node) const;
	bool IsConnectedToSegment(const URoadSegment* Segment) const;
};
