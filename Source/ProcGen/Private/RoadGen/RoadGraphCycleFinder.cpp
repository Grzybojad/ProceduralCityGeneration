// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadGen/RoadGraphCycleFinder.h"

TArray<TArray<FRoadNode*>> FRoadGraphCycleFinder::GetCycles(FRoadNetworkData* Network)
{
	TArray<TArray<FRoadNode*>> Cycles;
	TSet<TTuple<FRoadNode*,FRoadNode*>> VisitedEdges;

	for (const TSharedPtr<FRoadNode> NodePtr : Network->GetNodes())
	{
		FRoadNode* Node = NodePtr.Get();
		
		for (FRoadNode* Neighbour : Node->ConnectedNodes)
		{
			if(ContainsEdge(VisitedEdges, Node, Neighbour)) continue;
			
			TArray<FRoadNode*> CyclePath;
			const bool FoundLoop = CheckForCycleFromNode(Node, Neighbour, CyclePath);

			if(!FoundLoop) continue;

			// Add path edges to visited
			for(int i=0; i<CyclePath.Num()-1; ++i)
			{
				AddEdge(VisitedEdges, CyclePath[i], CyclePath[i+1]);
			}
			AddEdge(VisitedEdges, CyclePath[CyclePath.Num()-1], CyclePath[0]);
			Cycles.Add(CyclePath);
		}
	}

	return Cycles;
}

TArray<FVector2D> FRoadGraphCycleFinder::GetPlotPolygonFromCycle(TArray<FRoadNode*> Cycle, const float InsetAmount)
{
	const int VertexNum = Cycle.Num();
	TArray<FVector2D> CycleVertices;
	CycleVertices.SetNum(VertexNum);
	for(int i=0; i<VertexNum; ++i)
	{
		CycleVertices[i] = Cycle[i]->Position;
	}

	return FPcgUtils::ShrinkPolygon(CycleVertices, InsetAmount);
}

bool FRoadGraphCycleFinder::CheckForCycleFromNode(FRoadNode* StartNode, FRoadNode* FirstConnection, TArray<FRoadNode*>& Path)
{
	FRoadNode* CurrentNode = StartNode;
	FRoadNode* RightmostNeighbour = FirstConnection;
	
	Path.Add(CurrentNode);
	Path.Add(RightmostNeighbour);
	
	float TotalAngle = 0;
	
	constexpr int Limit = 200;
	int Loop = 0;
	
	do
	{
		const FRoadNode* PreviousNode = CurrentNode;
		CurrentNode = RightmostNeighbour;
		const FVector2D ReferenceBack = (PreviousNode->Position - CurrentNode->Position).GetSafeNormal();
		RightmostNeighbour = CurrentNode->GetRightMostNeighbour(ReferenceBack, PreviousNode);

		if(RightmostNeighbour == nullptr) return false;
		
		AddNodeToPath(PreviousNode, CurrentNode, RightmostNeighbour, Path, TotalAngle);
		Loop++;
	}
	while (RightmostNeighbour != StartNode && Loop < Limit);
	
	if(Loop >= Limit) return false;

	const float TurnAngle = FPcgUtils::AngleCCWBetweenNodes(CurrentNode, FirstConnection, StartNode);
	TotalAngle += TurnAngle;

	const float Vertices = Path.Num()-1;
	const float ExpectedTurn = (Vertices-2)*180;
	const bool IsValidPolygon = FMath::Abs(TotalAngle - ExpectedTurn) < 90;
	
	return IsValidPolygon;
}

void FRoadGraphCycleFinder::AddNodeToPath(const FRoadNode* PreviousNode, const FRoadNode* CurrentNode, FRoadNode* NewNode,TArray<FRoadNode*>& Path, float& TotalTurnAngle)
{
	const float TurnAngle = FPcgUtils::AngleCCWBetweenNodes(PreviousNode, NewNode, CurrentNode);
	TotalTurnAngle += TurnAngle;
	Path.Add(NewNode);
}

void FRoadGraphCycleFinder::AddEdge(TSet<TTuple<FRoadNode*, FRoadNode*>>& VisitedEdges, FRoadNode* Node1,
	FRoadNode* Node2)
{
	if(!ContainsEdge(VisitedEdges, Node1, Node2))
	{
		const TTuple<FRoadNode*,FRoadNode*> Edge = TTuple<FRoadNode*,FRoadNode*>(Node1, Node2);
		VisitedEdges.Add(Edge);
	}
}

bool FRoadGraphCycleFinder::ContainsEdge(const TSet<TTuple<FRoadNode*, FRoadNode*>>& VisitedEdges, FRoadNode* Node1,
										 FRoadNode* Node2)
{
	const TTuple<FRoadNode*,FRoadNode*> Edge = TTuple<FRoadNode*,FRoadNode*>(Node1, Node2);
	return VisitedEdges.Contains(Edge);
}

void FRoadGraphCycleFinder::DrawDebug(const UWorld* World, TArray<FVector2D> Plot, const float Z = 50)
{
	for (int i=0; i < Plot.Num(); ++i)
	{
		FVector LineStart = FVector(Plot[i].X, Plot[i].Y, Z);
		FVector LineEnd = FVector(Plot[(i+1) % Plot.Num()].X, Plot[(i+1) % Plot.Num()].Y, Z);
		DrawDebugLine(World, LineStart, LineEnd, FColor::Magenta, true, -1);
	}
}

void FRoadGraphCycleFinder::DrawDebug(const UWorld* World, TArray<FVector> Plot)
{
	for (int i=0; i < Plot.Num(); ++i)
	{
		FVector LineStart = Plot[i];
		FVector LineEnd = Plot[(i+1) % Plot.Num()];
		DrawDebugLine(World, LineStart, LineEnd, FColor::Magenta, true, -1);
	}
}

FBuildingPlot FRoadGraphCycleFinder::CycleToPlot(TArray<FVector2D> Cycle)
{
	FBuildingPlot Plot;

	for (int VertexIndex = 0; VertexIndex < Cycle.Num()-1; ++VertexIndex)
	{
		Plot.Outline.Add(FPcgUtils::FVector2DToFVector(Cycle[VertexIndex]));
	}
	return Plot;
}
