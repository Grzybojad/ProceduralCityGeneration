// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadGen/RoadGraphAgents.h"

URoadGraphAgents::URoadGraphAgents()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URoadGraphAgents::SetSeed(const int NewSeed)
{
	Seed = NewSeed;
	RandomStream = FRandomStream(Seed);
}

void URoadGraphAgents::GenerateRoadNetwork()
{
	Init();
	FRoadNode* StartNode = SetStartNode();
	AddFirstSegmentsToPlace(StartNode);

	// Road generation loop
	while (!SegmentsToPlace.IsEmpty() && Network->GetNodeIndexCounter() < SegmentsToGenerate-1)
	{
		FRoadSegmentToPlace R;
		SegmentsToPlace.HeapPop(R);
		
		if(LocalConstraints->HandleLocalConstraints(&R))
		{
			const URoadSegment* PlacedSegment = Network->PlaceSegment(R);

			if(PlacedSegment->End->GetMaxNewConnections(MaxNodeConnections) > 0)
			{
				AddSegmentToPlaceContinuingPathFromPlaced(R, PlacedSegment);
				AddBranchingOffSegmentsToPlace(R, PlacedSegment);
			}
		}
	}

	if(ExtendPathsToConnect)
	{
		ExtendPathsToConnectToGraph();
	}
}

FRoadPathsNodePositions URoadGraphAgents::GetRoadPaths() const
{
	FRoadPathsNodePositions PathsPositions;
	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	for (TSharedPtr<FRoadPath> PathPtr : Network->GetPaths())
	{
		const FRoadPath* RoadPath = PathPtr.Get();
		
		FRoadPathNodePositions NodePositions;
		
		for (const URoadSegment* RoadSegment : RoadPath->GetSegments())
		{
			FVector StartNodePos = FPcgUtils::FVector2DToFVector(RoadSegment->Start->Position) + OwnerLocation;
			FVector EndNodePos = FPcgUtils::FVector2DToFVector(RoadSegment->End->Position) + OwnerLocation;
		
			if(!NodePositions.Path.Contains(StartNodePos)) NodePositions.Path.Add(StartNodePos);
			if(!NodePositions.Path.Contains(EndNodePos)) NodePositions.Path.Add(EndNodePos);
		}

		if(NodePositions.Path.Num() > 1)
		{
			PathsPositions.Paths.Add(NodePositions);
		}
	}

	return PathsPositions;
}

FBuildingPlotsCollection URoadGraphAgents::GetBuildingPlots()
{
	PlotsCollection.BuildingPlots.Empty();
	TArray<TArray<FRoadNode*>> Cycles = FRoadGraphCycleFinder::GetCycles(Network);
	
	for (const TArray<FRoadNode*> Cycle : Cycles)
	{
		const TArray<FVector2D> PlotVertices = FRoadGraphCycleFinder:: GetPlotPolygonFromCycle(Cycle, PlotInset);
		FBuildingPlot Plot = FRoadGraphCycleFinder::CycleToPlot(PlotVertices);
		PlotsCollection.BuildingPlots.Add(Plot);
	}
	
	return PlotsCollection;
}

void URoadGraphAgents::Init()
{
	RandomStream = FRandomStream(Seed);
	
	if(Network == nullptr)
	{
		Network = new FRoadNetworkData();
	}
	else
	{
		Network->Reset();
	}
	if(LocalConstraints == nullptr)
	{
		LocalConstraints = new FLocalConstraints(MinNodeDistance, MinNodeToSegmentDistance,
					  NodeSnapDistance, MinConnectionAngle, MaxNodeConnections, Network);
	}
	SegmentsToPlace.Empty();
}

FRoadNode* URoadGraphAgents::SetStartNode() const
{
	FRoadNode* StartNode = new FRoadNode();
	Network->AddNode(StartNode);

	return StartNode;
}

void URoadGraphAgents::AddFirstSegmentsToPlace(FRoadNode* StartNode)
{
	FRoadSegmentToPlace FirstSegment = FRoadSegmentToPlace(0, StartNode, FVector2D(0, 0), Network->CreateAndAddNewPath());
	FirstSegment.SetRandomTargetInDistance(ChooseRandomSegmentDistance(), RandomStream);
	SegmentsToPlace.HeapPush(FirstSegment);

	// Add a branch in the opposite direction
	FRoadSegmentToPlace OppositeSegment = FRoadSegmentToPlace(1, StartNode, FVector2D(0, 0), Network->CreateAndAddNewPath());
	OppositeSegment.Target = StartNode->Position + (-FirstSegment.Target.GetSafeNormal() * ChooseRandomSegmentDistance());
	SegmentsToPlace.HeapPush(OppositeSegment);

	// Add the side branches
	constexpr int Branches = 2;
	TArray<FVector2D> BranchDirections = GetDirectionsForNewBranchesByPreviousDirection(FirstSegment.Target, Branches);
	
	for (int BranchIndex = 0; BranchIndex < Branches && Network->GetNodeIndexCounter() < SegmentsToGenerate-1; ++BranchIndex)
	{
		FRoadSegmentToPlace NewBranchSegment = FRoadSegmentToPlace(BranchIndex+1, StartNode, FVector2D(0, 0), Network->CreateAndAddNewPath());
		NewBranchSegment.Target = StartNode->Position + BranchDirections[BranchIndex] * ChooseRandomSegmentDistance();
		SegmentsToPlace.HeapPush(NewBranchSegment);
	}
}

void URoadGraphAgents::AddSegmentToPlaceContinuingPathFromPlaced(const FRoadSegmentToPlace OldSegment, const URoadSegment* PlacedSegment)
{
	// First segment continues path
	FRoadSegmentToPlace NewSegment = NewSegmentToPlaceContinueFromPrevious(OldSegment, PlacedSegment->End);

	const FVector2D PreviousDirection = PlacedSegment->GetDirection();
	bool Success = NewSegment.SetTargetToContinuePath(ChooseRandomSegmentDistance(), PreviousDirection, PathMaxTurnAngle, RandomStream);
				
	if(Success)
	{
		SegmentsToPlace.HeapPush(NewSegment);
	}
}

void URoadGraphAgents::AddBranchingOffSegmentsToPlace(const FRoadSegmentToPlace OldSegment, const URoadSegment* PlacedSegment)
{
	const int ExistingConnections = PlacedSegment->End->ConnectedNodes.Num();
	const int MaxBranchesToAdd = MaxNodeConnections-ExistingConnections-1;
	const int Branches = RandomStream.FRandRange(0, MaxBranchesToAdd);

	if(Branches == 0) return;
	
	TArray<FVector2D> BranchDirections = GetDirectionsForNewBranches(PlacedSegment, Branches);
	
	for (int BranchIndex = 0; BranchIndex < Branches && Network->GetNodeIndexCounter() < SegmentsToGenerate-1; ++BranchIndex)
	{
		FRoadSegmentToPlace NewBranchSegment = NewSegmentToPlaceBranchOffFromPrevious(OldSegment, PlacedSegment->End);
		NewBranchSegment.Target = PlacedSegment->End->Position + BranchDirections[BranchIndex] * ChooseRandomSegmentDistance();
		SegmentsToPlace.HeapPush(NewBranchSegment);
	}
}

TArray<FVector2D> URoadGraphAgents::GetDirectionsForNewBranches(const URoadSegment* PlacedSegment, const int Branches) const
{
	return GetDirectionsForNewBranchesByPreviousDirection(PlacedSegment->GetDirection(), Branches);
}

TArray<FVector2D> URoadGraphAgents::GetDirectionsForNewBranchesByPreviousDirection(const FVector2D PreviousDirection, const int Branches) const
{
	TArray<FVector2D> BranchesDirections;

	const bool IsOnLeftSide = RandomStream.FRandRange(0.0f, 1.0f) <= 0.5f;
	const int LeftRightModifier = IsOnLeftSide ? -1 : 1;

	float RandomAngle = RandomStream.FRandRange(BranchMinAngle, 180-BranchMinAngle) * LeftRightModifier;
	FVector2D RandomDirection = PreviousDirection.GetRotated(RandomAngle).GetSafeNormal();
	
	BranchesDirections.Add(RandomDirection);

	if(Branches > 1)
	{
		RandomAngle = RandomStream.FRandRange(BranchMinAngle, 180-BranchMinAngle) * -LeftRightModifier;
		RandomDirection = PreviousDirection.GetRotated(RandomAngle).GetSafeNormal();
		BranchesDirections.Add(RandomDirection);
	}

	// Shouldn't be used, but it's here just in case
	for (int i=0; i<Branches-2; ++i)
	{
		RandomDirection = PreviousDirection.GetRotated(RandomStream.FRandRange(0, 360)).GetSafeNormal();
		BranchesDirections.Add(RandomDirection);
	}

	return BranchesDirections;
}

float URoadGraphAgents::ChooseRandomSegmentDistance() const
{
	return RandomStream.FRandRange(MinRangeRoadSegmentLength, MaxRangeRoadSegmentLength);
}

FRoadSegmentToPlace URoadGraphAgents::NewSegmentToPlaceContinueFromPrevious(const FRoadSegmentToPlace OldSegment, FRoadNode* Start)
{
	return FRoadSegmentToPlace(OldSegment.Priority+1, Start, FVector2D(0, 0), OldSegment.Path);
}

FRoadSegmentToPlace URoadGraphAgents::NewSegmentToPlaceBranchOffFromPrevious(const FRoadSegmentToPlace OldSegment, FRoadNode* Start) const
{
	FRoadPath* NewPath = Network->CreateAndAddNewPath();
	return FRoadSegmentToPlace(OldSegment.Priority+1, Start, FVector2D(0, 0), NewPath);
}

void URoadGraphAgents::ExtendPathsToConnectToGraph()
{
	for (TSharedPtr<FRoadPath> PathPtr : Network->GetPaths())
	{
		FRoadPath* Path = PathPtr.Get();

		if(Path->Length() < 1) continue;
			
		const URoadSegment* LastSegment = Path->GetLastSegment();
		FRoadNode* LastNode = LastSegment->End;
			
		if(LastNode->State == DeadEnd)
		{
			constexpr float MaxDistance = 15000;
			FVector2D RayTarget = LastNode->Position + LastSegment->GetDirection().GetSafeNormal()*MaxDistance;
			FRoadSegmentToPlace R = FRoadSegmentToPlace(1, LastNode, RayTarget, Path);

			if(LocalConstraints->HandleLocalConstraints(&R, true))
			{
				if(R.ConstraintsState == ConnectedToExistingNode)
				{
					Network->PlaceSegment(R);
				}
			}
		}
	}
}


void URoadGraphAgents::DrawDebugGraph() const
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
}