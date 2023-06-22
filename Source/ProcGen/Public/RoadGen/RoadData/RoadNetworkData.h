#pragma once
#include "PCGUtils.h"
#include "RoadPath.h"
#include "RoadSegment.h"
#include "RoadSegmentToPlace.h"



class FRoadNetworkData
{
public:
	FRoadNetworkData(): NodeIndexCounter(0) {}
	
	void Reset();

	void AddRoadSegment(URoadSegment* Segment);
	void AddNode(FRoadNode* Node);
	FRoadPath* CreateAndAddNewPath();

	TArray<TWeakObjectPtr<URoadSegment>> GetRoadSegments();
	TArray<TSharedPtr<FRoadNode>> GetNodes();
	TArray<TSharedPtr<FRoadPath>> GetPaths();

	bool GetNodeByPosition(const FVector2D Position, FRoadNode*& FoundNode);
	bool GetSegmentByNodesPositions(const FVector2D Node1Position, const FVector2D Node2Position, URoadSegment*& FoundSegment);
	bool GetSegmentByStartEndPosition(const FVector2D StartPosition, const FVector2D EndPosition, URoadSegment*& FoundSegment);
	
	FRoadPath* GetPathWithSegment(const URoadSegment* Segment);

	int32 GetRoadSegmentsCount() const;
	
	URoadSegment* GetClosestSegmentFromNetwork(const FVector2D Point, float& Distance);
	
	URoadSegment* PlaceSegment(const FRoadSegmentToPlace& SegmentToPlace);

	int32 GetNodeIndexCounter() const;
	void IncrementNodeIndexCounter();

	bool AreNodesConnectedBySegment(const FRoadNode* NodeA, const FRoadNode* NodeB, URoadSegment*& ConnectingSegment);

protected:
	TArray<TWeakObjectPtr<URoadSegment>> RoadSegments;
	TArray<TSharedPtr<FRoadNode>> Nodes;
	TArray<TSharedPtr<FRoadPath>> Paths;
	
	int32 NodeIndexCounter;
	
};
