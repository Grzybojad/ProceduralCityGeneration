#include "RoadGen/RoadData/RoadSegmentToPlace.h"

FRoadSegmentToPlace::FRoadSegmentToPlace()
{
	Priority = 0;
	Start = nullptr;
	Target = FVector2D(0, 0);
	NodeToConnectTo = nullptr;
	ConstraintsState = Unmodified;
	Path = nullptr;
}

FRoadSegmentToPlace::FRoadSegmentToPlace(const int32 Priority, FRoadNode* Start, const FVector2d& Target, FRoadPath* Path):
	Priority(Priority), Start(Start), Target(Target), Path(Path), ConstraintsState(Unmodified), NodeToConnectTo(nullptr) {}

bool FRoadSegmentToPlace::operator<(const FRoadSegmentToPlace Other) const
{
	return Priority < Other.Priority;
}

void FRoadSegmentToPlace::SetRandomTargetInDistance(const float Distance, const FRandomStream RandomStream)
{
	const FVector2D StartPos = Start->Position;
	const FVector2D RandDirection = FVector2D(1, 0).GetRotated(RandomStream.FRandRange(0, 360));
	const FVector2D Offset = RandDirection * Distance;

	Target = StartPos + Offset;
}

bool FRoadSegmentToPlace::SetTargetToContinuePath(const float Distance, const FVector2D PreviousDirection, const float MaxTurnAngle, const FRandomStream RandomStream)
{
	const float RandomAngle = RandomStream.FRandRange(-MaxTurnAngle, MaxTurnAngle);
	const FVector2D TargetDir = PreviousDirection.GetRotated(RandomAngle).GetSafeNormal();

	if(TargetDir != FVector2D(0, 0))
	{
		const FVector2D StartPos = Start->Position;
		const FVector2D Offset = TargetDir * Distance;
		Target = StartPos + Offset;
		return true;
	}
	return false;
}

float FRoadSegmentToPlace::GetLength() const
{
	return (Target - Start->Position).Length();
}

FVector2D FRoadSegmentToPlace::GetDirection() const
{
	return (Target - Start->Position).GetSafeNormal();
}

bool FRoadSegmentToPlace::IsConnectedToNode(const FRoadNode* Node) const
{
	return Node == Start || (ConstraintsState == ConnectedToExistingNode && Node == NodeToConnectTo);
}

bool FRoadSegmentToPlace::IsConnectedToSegment(const URoadSegment* Segment) const
{
	return IsConnectedToNode(Segment->Start) || IsConnectedToNode(Segment->End);
}
