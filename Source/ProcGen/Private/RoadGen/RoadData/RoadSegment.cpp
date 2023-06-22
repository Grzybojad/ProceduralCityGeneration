#include "RoadGen/RoadData/RoadSegment.h"

void URoadSegment::SetNodesConnection() const
{
	SetNodesConnection(Start, End);
}

URoadSegment* URoadSegment::SplitSegment(const FVector2D SplitPoint, const int NewNodeIndex)
{
	// Current segment will be from Start to SplitPoint
	// New segment will be from SplitPoint to End

	FRoadNode* SplittingNode = new FRoadNode();
	SplittingNode->Index = NewNodeIndex;
	SplittingNode->Position = SplitPoint;
	SplittingNode->State = Crossing;
	SetNodesConnection(SplittingNode, Start);
	SetNodesConnection(SplittingNode, End);

	// The old connection is severed
	Start->ConnectedNodes.Remove(End);
	End->ConnectedNodes.Remove(Start);

	URoadSegment* NewSegment = NewObject<URoadSegment>();
	NewSegment->Start = SplittingNode;
	NewSegment->End = End;

	End = SplittingNode;

	return NewSegment;
}

FVector2D URoadSegment::GetDirection() const
{
	return End->Position - Start->Position;
}

void URoadSegment::SetNodesConnection(FRoadNode* NodeA, FRoadNode* NodeB)
{
	NodeA->ConnectedNodes.Add(NodeB);
	NodeB->ConnectedNodes.Add(NodeA);
}
