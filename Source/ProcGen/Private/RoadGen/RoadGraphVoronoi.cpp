// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadGen/RoadGraphVoronoi.h"

URoadGraphVoronoi::URoadGraphVoronoi()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URoadGraphVoronoi::SetSeed(const int NewSeed)
{
	Seed = NewSeed;
	RandomStream = FRandomStream(Seed);
}

void URoadGraphVoronoi::GenerateRoadNetwork()
{
	RandomStream = FRandomStream(Seed);

	VoronoiSites = GenerateSites();
	if(SquareGrid)
	{
		Bounds = FVoronoiDiagram::GetBounds(VoronoiSites, MaxDistance/NrOfSites);
	}
	else
	{
		Bounds = FVoronoiDiagram::GetBounds(VoronoiSites, MaxDistance/10);
	}
	
	GetVoronoiEdges(VoronoiSites, Bounds, VoronoiEdges, VoronoiCellMember, 0.1f);

	Network = CreateNetworkDataFromVoronoiGraph();
}

FRoadPathsNodePositions URoadGraphVoronoi::GetRoadPaths() const
{
	FRoadPathsNodePositions RoadPathsNodePositions;
	
	for (TWeakObjectPtr<URoadSegment> RoadSegmentPtr : Network->GetRoadSegments())
	{
		const URoadSegment* Segment = RoadSegmentPtr.Get();
		
		FRoadPathNodePositions PathPositions;
		FVector StartPos = FPcgUtils::FVector2DToFVector(Segment->Start->Position);
		FVector EndPos = FPcgUtils::FVector2DToFVector(Segment->End->Position);
		PathPositions.Path.Add(StartPos);
		PathPositions.Path.Add(EndPos);
		RoadPathsNodePositions.Paths.Add(PathPositions);
	}

	return RoadPathsNodePositions;
}

FBuildingPlotsCollection URoadGraphVoronoi::GetBuildingPlots()
{
	PlotsCollection.BuildingPlots.Empty();
	TArray<TArray<FRoadNode*>> Cycles = FRoadGraphCycleFinder::GetCycles(Network);

	for (const TArray<FRoadNode*> Cycle : Cycles)
	{
		const TArray<FVector2D> PlotVertices = FRoadGraphCycleFinder::GetPlotPolygonFromCycle(Cycle, RoadWidth);
		FBuildingPlot Plot = FRoadGraphCycleFinder::CycleToPlot(PlotVertices);
		PlotsCollection.BuildingPlots.Add(Plot);
	}

	return PlotsCollection;
}


TArray<FVector> URoadGraphVoronoi::GenerateSites() const
{
	TArray<FVector> Sites;
	
	if(SquareGrid)
	{
		const float Offset = - MaxDistance + (MaxDistance/NrOfSites);
		for (int SiteIndex = 0; SiteIndex < NrOfSites*NrOfSites; ++SiteIndex)
		{
			const float Xt = static_cast<float>(SiteIndex % NrOfSites) / NrOfSites;
			const float Xy = static_cast<float>(SiteIndex / NrOfSites) / NrOfSites;
			const float X = Xt * MaxDistance * 2 + Offset;
			const float Y = Xy * MaxDistance * 2 + Offset;
			Sites.Add(FVector(X, Y, 0));
		}
	}
	else
	{
		Sites.Init(FVector(0, 0, 0), NrOfSites);
		
		for (int SiteIndex = 0; SiteIndex < NrOfSites; ++SiteIndex)
		{
			const float RandX = RandomStream.FRandRange(-MaxDistance, MaxDistance);
			const float RandY = RandomStream.FRandRange(-MaxDistance, MaxDistance);
			Sites.Add(FVector(RandX, RandY, 0));
		}
	}

	return Sites;
}

FRoadNetworkData* URoadGraphVoronoi::CreateNetworkDataFromVoronoiGraph() const
{
	FRoadNetworkData* RoadDataNetwork = new FRoadNetworkData();

	float MinX = 999999; float MinY = 999999;
	float MaxX = 0; float MaxY = 0;
	for (const auto Edge : VoronoiEdges)
	{
		const FVector2D Node1Pos = FPcgUtils::FVectorToFVector2D(Edge.Key);
		const FVector2D Node2Pos = FPcgUtils::FVectorToFVector2D(Edge.Value);
		if(Node1Pos.X < MinX) MinX = Node1Pos.X;
		if(Node2Pos.X < MinX) MinX = Node2Pos.X;
		if(Node1Pos.Y < MinY) MinY = Node1Pos.Y;
		if(Node2Pos.Y < MinY) MinY = Node2Pos.Y;
		if(Node1Pos.X > MaxX) MaxX = Node1Pos.X;
		if(Node2Pos.X > MaxX) MaxX = Node2Pos.X;
		if(Node1Pos.Y > MaxY) MaxY = Node1Pos.Y;
		if(Node2Pos.Y > MaxY) MaxY = Node2Pos.Y;
	}
	TSet FrameX = {MinX, MaxX};
	TSet FrameY = {MinY, MaxY};
	
	int Index = 0;
	for (const auto Edge : VoronoiEdges)
	{
		const FVector2D Node1Pos = FPcgUtils::FVectorToFVector2D(Edge.Key);
		const FVector2D Node2Pos = FPcgUtils::FVectorToFVector2D(Edge.Value);
		constexpr float Epsilon = 0.0001;
		
		if( FMath::Abs(Node1Pos.X - Node2Pos.X) < Epsilon &&
			FMath::Abs(Node1Pos.Y - Node2Pos.Y) < Epsilon) continue;
		
		if(RemoveFrame)
		{
			if( FrameX.Contains(Node1Pos.X) || FrameX.Contains(Node2Pos.X) ||
				FrameY.Contains(Node1Pos.Y) || FrameY.Contains(Node2Pos.Y))
			{
				continue;
			}
		}
		
		URoadSegment* RoadSegment = NewObject<URoadSegment>();
		if(RoadDataNetwork->GetSegmentByNodesPositions(Node1Pos, Node2Pos, RoadSegment)) continue;

		FRoadNode* StartNode;
		if(!RoadDataNetwork->GetNodeByPosition(Node1Pos, StartNode))
		{
			StartNode = new FRoadNode(Index++, Node1Pos, Extension);
			RoadDataNetwork->AddNode(StartNode);
		}
		
		FRoadNode* EndNode;
		if(!RoadDataNetwork->GetNodeByPosition(Node2Pos, EndNode))
		{
			EndNode = new FRoadNode(Index++, Node2Pos, Extension);
			RoadDataNetwork->AddNode(EndNode);
		}

		RoadSegment->Start = StartNode;
		RoadSegment->End = EndNode;
		RoadSegment->SetNodesConnection();
		RoadDataNetwork->AddRoadSegment(RoadSegment);
	}

	return RoadDataNetwork;
}

void URoadGraphVoronoi::DrawDebugGraph() const
{
	constexpr float Z = 50;
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	
	for (TWeakObjectPtr<URoadSegment> RoadSegmentPtr : Network->GetRoadSegments())
	{
		const URoadSegment* RoadSegment = RoadSegmentPtr.Get();
		
		const FVector2D Start = RoadSegment->Start->Position;
		const FVector2D End = RoadSegment->End->Position;
		FVector LineStart = FVector(Start.X, Start.Y, Z) + OwnerLocation;
		FVector LineEnd = FVector(End.X, End.Y, Z) + OwnerLocation;
		
		DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::White, true, -1);
		FVector IndicatorPos = FMath::Lerp(LineStart, LineEnd, 0.8f);
		DrawDebugSphere(GetWorld(), IndicatorPos, 10.0f, 8, FColor::Purple, true, -1);
	}

	for (const TSharedPtr<FRoadNode> NodePtr : Network->GetNodes())
	{
		const FRoadNode* Node = NodePtr.Get();
		
		FVector Position = FVector(Node->Position.X, Node->Position.Y, Z) + OwnerLocation;
		DrawDebugSphere(GetWorld(), Position, 30.0f, 12, Node->GetColorByState(), true, -1);
	}

	for (FVector Site : VoronoiSites)
	{
		DrawDebugSphere(GetWorld(), Site, 60.0f, 12, FColor::Magenta, true, -1);
	}

	for (const FBuildingPlot BuildingPlot : PlotsCollection.BuildingPlots)
	{
		FRoadGraphCycleFinder::DrawDebug(GetWorld(), BuildingPlot.Outline);
	}
}
