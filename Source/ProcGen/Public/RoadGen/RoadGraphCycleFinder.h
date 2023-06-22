// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoadData/BuildingPlot.h"
#include "RoadData/RoadNetworkData.h"
#include "RoadData/RoadNode.h"

class PROCGEN_API FRoadGraphCycleFinder
{
public:
	static TArray<TArray<FRoadNode*>> GetCycles(FRoadNetworkData* Network);

	static TArray<FVector2D> GetPlotPolygonFromCycle(TArray<FRoadNode*> Cycle, float InsetAmount);

	static void DrawDebug(const UWorld* World, TArray<FVector2D> Plot, const float Z);
	static void DrawDebug(const UWorld* World, TArray<FVector> Plot);

	static FBuildingPlot CycleToPlot(TArray<FVector2D> Cycle);

protected:
	static bool CheckForCycleFromNode(FRoadNode* StartNode, FRoadNode* FirstConnection, TArray<FRoadNode*>& Path);
	
	static void AddNodeToPath(const FRoadNode* PreviousNode, const FRoadNode* CurrentNode, FRoadNode* NewNode, TArray<FRoadNode*>& Path, float&
							  TotalTurnAngle);

	static void AddEdge(TSet<TTuple<FRoadNode*, FRoadNode*>>& VisitedEdges, FRoadNode* Node1, FRoadNode* Node2);
	
	static bool ContainsEdge(const TSet<TTuple<FRoadNode*, FRoadNode*>>& VisitedEdges, FRoadNode* Node1, FRoadNode* Node2);
};
