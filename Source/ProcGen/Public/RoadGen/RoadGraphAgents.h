// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RoadData/RoadNetworkData.h"
#include "RoadData/RoadNode.h"
#include "RoadData/RoadSegmentToPlace.h"
#include "RoadData/RoadSegment.h"
#include "RoadGen/RoadGraphCycleFinder.h"
#include "RoadGen/RoadData/RoadPathsNodePositions.h"
#include "LocalConstraints.h"
#include "PCGUtils.h"
#include "RoadData/BuildingPlotsCollection.h"
#include "RoadGraphAgents.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCGEN_API URoadGraphAgents : public UActorComponent
{
	GENERATED_BODY()

public:	
	URoadGraphAgents();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0, ClampMax=1000))
	int32 SegmentsToGenerate = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float MinRangeRoadSegmentLength = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float MaxRangeRoadSegmentLength = 1500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float MinNodeDistance = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float MinNodeToSegmentDistance = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float NodeSnapDistance = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0, ClampMax=180))
	float PathMaxTurnAngle = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0, ClampMax=90))
	float BranchMinAngle = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0, ClampMax=90))
	float MinConnectionAngle = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=2, ClampMax=8))
	int MaxNodeConnections = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float PlotInset = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ExtendPathsToConnect = false;

	
	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Road Agents")
	void SetSeed(const int NewSeed);

	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Road Agents")
	void GenerateRoadNetwork();
	
	UFUNCTION(BlueprintPure, Category="Procedural Generation|Road Agents")
	FRoadPathsNodePositions GetRoadPaths() const;

	UFUNCTION(BlueprintPure, Category="Procedural Generation|Road Agents")
	FBuildingPlotsCollection GetBuildingPlots();

	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Road Agents")
	void DrawDebugGraph() const;

protected:
	UPROPERTY(VisibleAnywhere)
	int Seed = 0;
	FRandomStream RandomStream;
	
	FRoadNetworkData* Network;
	FLocalConstraints* LocalConstraints;
	
	TArray<FRoadSegmentToPlace> SegmentsToPlace;

	FBuildingPlotsCollection PlotsCollection;

	void Init();
	FRoadNode* SetStartNode() const;
	void AddFirstSegmentsToPlace(FRoadNode* StartNode);

	void AddSegmentToPlaceContinuingPathFromPlaced(const FRoadSegmentToPlace OldSegment, const URoadSegment* PlacedSegment);
	
	void AddBranchingOffSegmentsToPlace(const FRoadSegmentToPlace OldSegment, const URoadSegment* PlacedSegment);

	TArray<FVector2D> GetDirectionsForNewBranches(const URoadSegment* PlacedSegment, const int Branches) const;
	
	TArray<FVector2D> GetDirectionsForNewBranchesByPreviousDirection(const FVector2D PreviousDirection, const int Branches) const;

	float ChooseRandomSegmentDistance() const;

	static FRoadSegmentToPlace NewSegmentToPlaceContinueFromPrevious(const FRoadSegmentToPlace OldSegment, FRoadNode* Start);

	FRoadSegmentToPlace NewSegmentToPlaceBranchOffFromPrevious(const FRoadSegmentToPlace OldSegment, FRoadNode* Start) const;

	void ExtendPathsToConnectToGraph();
};
