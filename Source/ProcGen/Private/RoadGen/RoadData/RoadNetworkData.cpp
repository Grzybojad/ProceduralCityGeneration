#include "RoadGen/RoadData/RoadNetworkData.h"


void FRoadNetworkData::Reset()
{
	for (TWeakObjectPtr<URoadSegment> RoadSegmentPtr : RoadSegments)
	{
		if(RoadSegmentPtr.IsValid())
		{
			URoadSegment* RoadSegment = RoadSegmentPtr.Get();
			RoadSegment->ConditionalBeginDestroy();
		}
	}
	RoadSegments.Empty();
	Nodes.Empty();
	Paths.Empty();
	NodeIndexCounter = 0;
}

void FRoadNetworkData::AddRoadSegment(URoadSegment* Segment)
{
	RoadSegments.Add(Segment);
}

void FRoadNetworkData::AddNode(FRoadNode* Node)
{
	Nodes.Add(MakeShareable(Node));
}

FRoadPath* FRoadNetworkData::CreateAndAddNewPath()
{
	const TSharedPtr<FRoadPath> Path = MakeShared<FRoadPath>();
	Paths.Add(Path);
	return Path.Get();
}

TArray<TWeakObjectPtr<URoadSegment>> FRoadNetworkData::GetRoadSegments()
{
	return RoadSegments;
}

TArray<TSharedPtr<FRoadNode>> FRoadNetworkData::GetNodes()
{
	return Nodes;
}

TArray<TSharedPtr<FRoadPath>> FRoadNetworkData::GetPaths()
{
	return Paths;
}

bool FRoadNetworkData::GetNodeByPosition(const FVector2D Position, FRoadNode*& FoundNode)
{
	constexpr float Epsilon = 0.0001f;
	
	for (TSharedPtr<FRoadNode> NodePtr : Nodes)
	{
		FRoadNode* Node = NodePtr.Get();
		
		if( FMath::Abs(Node->Position.X - Position.X) < Epsilon &&
			FMath::Abs(Node->Position.Y - Position.Y) < Epsilon)
		{
			FoundNode = Node;
			return true;
		}
	}

	return false;
}

bool FRoadNetworkData::GetSegmentByNodesPositions(const FVector2D Node1Position, const FVector2D Node2Position,
	URoadSegment*& FoundSegment)
{
	return
		GetSegmentByStartEndPosition(Node1Position, Node2Position, FoundSegment) ||
		GetSegmentByStartEndPosition(Node2Position, Node1Position, FoundSegment);
}

bool FRoadNetworkData::GetSegmentByStartEndPosition(const FVector2D StartPosition, const FVector2D EndPosition,
                                                    URoadSegment*& FoundSegment)
{
	constexpr float Epsilon = 0.0001f;
	
	for (const TWeakObjectPtr<URoadSegment> Segment : RoadSegments)
	{
		const FVector2D SegmentStartPos = Segment->Start->Position;
		const FVector2D SegmentEndPos = Segment->End->Position;
		
		if( 
			FMath::Abs(SegmentStartPos.X - StartPosition.X) < Epsilon &&
			FMath::Abs(SegmentStartPos.Y - StartPosition.Y) < Epsilon &&
			FMath::Abs(SegmentEndPos.X - EndPosition.X) < Epsilon &&
			FMath::Abs(SegmentEndPos.Y - EndPosition.Y) < Epsilon)
		{
			FoundSegment = Segment.Get();
			return true;
		}
	}

	return false;
}


FRoadPath* FRoadNetworkData::GetPathWithSegment(const URoadSegment* Segment)
{
	for (TSharedPtr<FRoadPath> PathPtr : GetPaths())
	{
		FRoadPath* Path = PathPtr.Get();
		
		for (const URoadSegment* SegmentOfPath : Path->GetSegments())
		{
			if(SegmentOfPath == Segment) return Path;
		}
	}

	return nullptr;
}

int32 FRoadNetworkData::GetRoadSegmentsCount() const
{
	return RoadSegments.Num();
}

URoadSegment* FRoadNetworkData::GetClosestSegmentFromNetwork(const FVector2D Point, float& Distance)
{
	if(RoadSegments.IsEmpty()) return nullptr;
	
	Distance = 9999999999999.9f;
	TWeakObjectPtr<URoadSegment> ClosestSegment = RoadSegments.Top();
	
	for (TWeakObjectPtr<URoadSegment> RoadSegment : RoadSegments)
	{
		const float DistanceToSegment = FPcgUtils::PointToRoadSegmentDistance(RoadSegment.Get(), Point);

		if(DistanceToSegment < Distance)
		{
			Distance = DistanceToSegment;
			ClosestSegment = RoadSegment;
		}
	}

	return ClosestSegment.Get();
}

URoadSegment* FRoadNetworkData::PlaceSegment(const FRoadSegmentToPlace& SegmentToPlace)
{
	URoadSegment* NewRoadSegment = NewObject<URoadSegment>();
	NewRoadSegment->Start = SegmentToPlace.Start;
	
	if(SegmentToPlace.ConstraintsState == ConnectedToExistingNode)
	{
		// Check if the two nodes were already connected
		URoadSegment* ConnectingSegment;
		if(AreNodesConnectedBySegment(SegmentToPlace.Start, SegmentToPlace.NodeToConnectTo, ConnectingSegment))
		{
			return ConnectingSegment;
		}
		
		// Connect to the existing node
		NewRoadSegment->End = SegmentToPlace.NodeToConnectTo;
		NewRoadSegment->End->State = Crossing;
	}
	else
	{
		IncrementNodeIndexCounter();
		
		// Create a new node
		FRoadNode* EndNode = new FRoadNode(NodeIndexCounter, SegmentToPlace.Target, DeadEnd);
		AddNode(EndNode);
		
		NewRoadSegment->End = EndNode;

		if(SegmentToPlace.Start->State == Disconnected) SegmentToPlace.Start->State = DeadEnd;
		else if(SegmentToPlace.Start->State == DeadEnd) SegmentToPlace.Start->State = Extension;
	}

	NewRoadSegment->SetNodesConnection();
	RoadSegments.Add(NewRoadSegment);
	SegmentToPlace.Path->AddSegment(NewRoadSegment);

	return NewRoadSegment;
}

int32 FRoadNetworkData::GetNodeIndexCounter() const
{
	return NodeIndexCounter;
}

void FRoadNetworkData::IncrementNodeIndexCounter()
{
	NodeIndexCounter++;
}

bool FRoadNetworkData::AreNodesConnectedBySegment(const FRoadNode* NodeA, const FRoadNode* NodeB, URoadSegment*& ConnectingSegment)
{
	for (TWeakObjectPtr<URoadSegment> RoadSegmentPtr : GetRoadSegments())
	{
		URoadSegment* Segment = RoadSegmentPtr.Get();
		
		if( Segment->Start == NodeA && Segment->End == NodeB ||
			Segment->Start == NodeB && Segment->End == NodeA)
		{
			ConnectingSegment = Segment;
			return true;
		}
	}

	return false;
}
