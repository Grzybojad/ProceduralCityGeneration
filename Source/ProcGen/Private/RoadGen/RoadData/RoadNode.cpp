// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadGen/RoadData/RoadNode.h"

FRoadNode::FRoadNode()
{
	Index = 0;
	Position = FVector2d(0, 0);
	State = Disconnected;
}

FRoadNode::FRoadNode(const int32 Index, const FVector2D Position, const TEnumAsByte<ENodeState> State):
	Index(Index), Position(Position), State(State){}

bool FRoadNode::IsConnectedTo(const FRoadNode* Node) const
{
	return ConnectedNodes.Contains(Node);
}

int FRoadNode::GetNumberOfConnectedNodes() const
{
	return ConnectedNodes.Num();
}

int FRoadNode::GetMaxNewConnections(const int ConnectionsLimit) const
{
	return ConnectionsLimit - GetNumberOfConnectedNodes();
}

TArray<FRoadNode*> FRoadNode::GetConnectedInDirection(const FVector2D Direction, const float MaxAngle)
{
	TArray<FRoadNode*> Nodes;
	
	for (FRoadNode* Neighbour : ConnectedNodes)
	{
		FVector2D NeighbourDirection = (Neighbour->Position - Position).GetSafeNormal();
		const float Dot = FVector2D::DotProduct(NeighbourDirection, Direction);
		const float Angle = FMath::Acos(Dot) / PI * 180;
		
		if(Angle < MaxAngle)
		{
			Nodes.Add(Neighbour);
		}
	}

	return Nodes;
}

FRoadNode* FRoadNode::GetRightMostNeighbour(const FVector2D ReferenceBack, const FRoadNode* ToIgnore)
{
	float BestScore = -10;
	FRoadNode* RightmostNode = nullptr;
	
	for (FRoadNode* Neighbour : ConnectedNodes)
	{
		if(Neighbour == ToIgnore) continue;
		
		FVector2D NeighbourDirection = (Neighbour->Position - Position).GetSafeNormal();

		// Based on https://stackoverflow.com/questions/13221873/determining-if-one-2d-vector-is-to-the-right-or-left-of-another
		// if 90 deg on right: score = 0
		// if in front: score = -1
		// if on right and (almost) same as reference back: score = 1
		// if 90 deg on left: score = -2
		// if on left and (almost) same as reference back: score = -3
		FVector2D Rotated90CCW = NeighbourDirection.GetRotated(-90);
		const bool OnLeft = FVector2D::DotProduct(ReferenceBack, Rotated90CCW) < 0;
		float Score = FVector2D::DotProduct(ReferenceBack, NeighbourDirection);
		if(OnLeft) Score = -Score - 2;

		if(Score > BestScore)
		{
			RightmostNode = Neighbour;
			BestScore = Score;
		}
	}

	return RightmostNode;
}

FColor FRoadNode::GetColorByState() const
{
	if(State == Disconnected) return FColor::Orange;
	if(State == DeadEnd) return FColor::Red;
	if(State == Extension) return FColor::Green;
	if(State == Crossing) return FColor::Blue;

	return FColor::Magenta;
}


