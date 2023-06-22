#include "RoadGen/LocalConstraints.h"



bool FLocalConstraints::HandleLocalConstraints(FRoadSegmentToPlace* SegmentToPlace, const bool IgnoreLimits) const
{
	if(Network->GetRoadSegmentsCount() == 0) return true;

	TArray<URoadSegment*> IntersectedSegments;
	const bool Intersecting = IsIntersectingSegments(SegmentToPlace, IntersectedSegments);
	
	if(Intersecting)
	{
		const bool CanBePlaced = HandleIntersections(SegmentToPlace, IntersectedSegments, IgnoreLimits);

		if(!CanBePlaced) return false;
	}
	else
	{
		const bool CanBePlaced = HandleNotIntersectingLine(SegmentToPlace);

		if(!CanBePlaced) return false;
	}

	return IgnoreLimits ? true : FinalCheck(SegmentToPlace);
}

bool FLocalConstraints::HandleIntersections(FRoadSegmentToPlace* SegmentToPlace, const TArray<URoadSegment*>& IntersectedSegments, const bool IgnoreLimits) const
{
	URoadSegment* ClosestSegment = GetClosest(SegmentToPlace->Start->Position, IntersectedSegments);

	// Snap to the closest intersected segment
	FVector2D IntersectionPoint;
	FPcgUtils::RoadSegmentIntersection(SegmentToPlace, ClosestSegment, IntersectionPoint);

	if(!IgnoreLimits)
	{
		if( FVector2D::Distance(IntersectionPoint, ClosestSegment->Start->Position) < MinNodeDistance ||
		FVector2D::Distance(IntersectionPoint, ClosestSegment->End->Position) < MinNodeDistance ||
		FVector2D::Distance(IntersectionPoint, SegmentToPlace->Start->Position) < MinNodeDistance) 
		{
			return false;
		}
	}

	SegmentToPlace->Target = IntersectionPoint;
	SplitSegment(ClosestSegment, SegmentToPlace);
	SegmentToPlace->ConstraintsState = ConnectedToExistingNode;
	
	return true;
}

URoadSegment* FLocalConstraints::GetClosest(const FVector2D StartPos, const TArray<URoadSegment*>& IntersectedSegments)
{
	URoadSegment* ClosestSegment = IntersectedSegments[0];
		
	if(IntersectedSegments.Num() > 1)
	{
		// Get closest intersected
		float MinDistance = 9999999999;
		
		for (URoadSegment* IntersectedSegment : IntersectedSegments)
		{
			const float Distance = FPcgUtils::PointToRoadSegmentDistance(IntersectedSegment, StartPos);

			if(Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestSegment = IntersectedSegment;
			}
		}
	}

	return ClosestSegment;
}

bool FLocalConstraints::HandleNotIntersectingLine(FRoadSegmentToPlace* SegmentToPlace) const
{
	float DistanceToSegment;
	URoadSegment* ClosestSegment = Network->GetClosestSegmentFromNetwork(SegmentToPlace->Target, DistanceToSegment);

	if(SegmentToPlace->Start == ClosestSegment->Start || SegmentToPlace->Start == ClosestSegment->End)
	{
		// Closest segment is attached to our starting point
		const float Distance = FPcgUtils::PointToRoadSegmentDistance(ClosestSegment, SegmentToPlace->Target);
		
		if(Distance < MinNodeToSegmentDistance)
		{
			// Node would be too close to this segment
			return false;
		}
	}
	
	if(DistanceToSegment < NodeSnapDistance)
	{
		const bool CanConnectToStart = ClosestSegment->Start->GetMaxNewConnections(MaxNodeConnections) > 0;
		const bool CanConnectToEnd = ClosestSegment->End->GetMaxNewConnections(MaxNodeConnections) > 0;
		const float SegmentStartDistance = FVector2D::Distance(SegmentToPlace->Target, ClosestSegment->Start->Position);
		const float SegmentEndDistance = FVector2D::Distance(SegmentToPlace->Target, ClosestSegment->End->Position);

		if(CanConnectToStart && SegmentStartDistance < SegmentEndDistance && SegmentStartDistance < NodeSnapDistance)
		{
			// Snap to start
			return SnapToNode(SegmentToPlace, ClosestSegment->Start);
		}
		else if(CanConnectToEnd && SegmentEndDistance < NodeSnapDistance)
		{
			// Snap to end
			return SnapToNode(SegmentToPlace, ClosestSegment->End);
		}
		else
		{
			return ExtendToReachSegment(SegmentToPlace, ClosestSegment);
		}
	}

	return true;
}

bool FLocalConstraints::IsIntersectingSegments(const FRoadSegmentToPlace* SegmentToPlace, TArray<URoadSegment*>& IntersectedSegments) const
{
	for (TWeakObjectPtr<URoadSegment> RoadSegmentPtr : Network->GetRoadSegments())
	{
		URoadSegment* RoadSegment = RoadSegmentPtr.Get();
		FVector2D IntersectionPoint;
		const bool Intersected = FPcgUtils::RoadSegmentIntersection(SegmentToPlace, RoadSegment, IntersectionPoint);

		if(Intersected)
		{
			IntersectedSegments.Add(RoadSegment);
		}
	}
	
	return !IntersectedSegments.IsEmpty();
}

bool FLocalConstraints::SnapToNode(FRoadSegmentToPlace* SegmentToPlace, FRoadNode* NodeToSnapTo) const
{
	SegmentToPlace->ConstraintsState = ConnectedToExistingNode;
	SegmentToPlace->NodeToConnectTo = NodeToSnapTo;
	SegmentToPlace->Target = NodeToSnapTo->Position;

	const FVector2D ReverseDirection = -SegmentToPlace->GetDirection();
	
	for (const FRoadNode* ConnectedNode : NodeToSnapTo->ConnectedNodes)
	{
		const FVector2D Direction = (ConnectedNode->Position - NodeToSnapTo->Position).GetSafeNormal();
	
		const float Dot = ReverseDirection.Dot(Direction);
		const float Angle = (FMath::Acos(Dot) / PI) * 180;
		
		if(Angle < MinConnectionAngle)
		{
			return false;
		}
	}

	return true;
}

bool FLocalConstraints::ExtendToReachSegment(FRoadSegmentToPlace* SegmentToPlace, URoadSegment* SegmentToReach) const
{
	const FVector2D SegmentToPlaceStart = SegmentToPlace->Start->Position;
	const FVector2D SegmentToPlaceTarget = SegmentToPlace->Target;
	const FVector2D SegmentToReachStart = SegmentToReach->Start->Position;
	const FVector2D SegmentToReachEnd = SegmentToReach->End->Position;
	
	FVector2D ExtendedTargetToReachSegment;
	const bool LinesNotParallel = FPcgUtils::LineToLineIntersection(
		SegmentToReachStart, SegmentToPlaceStart,
		SegmentToReachEnd, SegmentToPlaceTarget, ExtendedTargetToReachSegment);


	if(!LinesNotParallel) return false;
	
	const float ExtendedSegmentLength = FVector2D::Distance(SegmentToPlaceStart, ExtendedTargetToReachSegment);
		
	if(ExtendedSegmentLength > SegmentToPlace->GetLength() + NodeSnapDistance)
	{
		// Extension would be too long
		return false;
	}

	// Too close to existing nodes
	if( FVector2D::Distance(ExtendedTargetToReachSegment, SegmentToReach->Start->Position) < MinNodeDistance ||
		FVector2D::Distance(ExtendedTargetToReachSegment, SegmentToReach->End->Position) < MinNodeDistance)
	{
		return false;
	}

	// Split the reached segment at target point
	SegmentToPlace->Target = ExtendedTargetToReachSegment;
	SplitSegment(SegmentToReach, SegmentToPlace);
	SegmentToPlace->ConstraintsState = ConnectedToExistingNode;

	return true;
}

// Splits Segment into two, Segment end will be attached to the new SplittingNode and
// NewSegment will be a from SplittingNode to the previous End
void FLocalConstraints::SplitSegment(URoadSegment* SegmentBeingSplit, FRoadSegmentToPlace* SplittingSegment) const
{
	Network->IncrementNodeIndexCounter();
	URoadSegment* NewSegment = SegmentBeingSplit->SplitSegment(SplittingSegment->Target, Network->GetNodeIndexCounter());
	Network->AddRoadSegment(NewSegment);
	Network->GetPathWithSegment(SegmentBeingSplit)->SegmentWasSplit(SegmentBeingSplit, NewSegment);
	
	FRoadNode* SplittingNode = NewSegment->Start;
	Network->AddNode(SplittingNode);
	SplittingSegment->ConstraintsState = ConnectedToExistingNode;
	SplittingSegment->NodeToConnectTo = SplittingNode;
}

bool FLocalConstraints::FinalCheck(const FRoadSegmentToPlace* SegmentToPlace) const
{
	// Check if it's not too close to an existing segment
	for (TWeakObjectPtr<URoadSegment> RoadSegmentPtr : Network->GetRoadSegments())
	{
		const URoadSegment* Segment = RoadSegmentPtr.Get();
		
		if(SegmentToPlace->IsConnectedToSegment(Segment)) continue;

		const float DistanceToStart = FPcgUtils::PointToRoadSegmentDistance(Segment, SegmentToPlace->Start->Position);
		if(DistanceToStart < MinNodeToSegmentDistance) return false;

		const float DistanceToTarget = FPcgUtils::PointToRoadSegmentDistance(Segment, SegmentToPlace->Target);
		if(DistanceToTarget < MinNodeToSegmentDistance) return false;

		const float DistanceFromOtherStart = FPcgUtils::PointToLineSegmentDistance(SegmentToPlace->Start->Position, SegmentToPlace->Target, Segment->Start->Position);
		if(DistanceFromOtherStart < MinNodeToSegmentDistance) return false;

		const float DistanceFromOtherEnd = FPcgUtils::PointToLineSegmentDistance(SegmentToPlace->Target, SegmentToPlace->Target, Segment->Start->Position);
		if(DistanceFromOtherEnd < MinNodeToSegmentDistance) return false;
	}

	return true;
}
