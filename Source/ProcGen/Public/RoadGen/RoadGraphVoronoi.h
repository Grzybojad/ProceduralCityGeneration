// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Voronoi/Voronoi.h"
#include "PCGUtils.h"
#include "RoadData/BuildingPlotsCollection.h"
#include "RoadGen/RoadGraphCycleFinder.h"
#include "RoadGen/RoadData/RoadNetworkData.h"
#include "RoadGen/RoadData/RoadPathsNodePositions.h"
#include "RoadGen/RoadData/RoadSegment.h"
#include "RoadGraphVoronoi.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCGEN_API URoadGraphVoronoi : public UActorComponent
{
	GENERATED_BODY()

public:	
	URoadGraphVoronoi();

	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Road Voronoi")
	void SetSeed(const int NewSeed);
	
	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Road Voronoi")
	void GenerateRoadNetwork();

	UFUNCTION(BlueprintPure, Category="Procedural Generation|Road Voronoi")
	FRoadPathsNodePositions GetRoadPaths() const;

	UFUNCTION(BlueprintPure, Category="Procedural Generation|Road Voronoi")
	FBuildingPlotsCollection GetBuildingPlots();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	int NrOfSites = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float MaxDistance = 5000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float RoadWidth = 370;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SquareGrid = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RemoveFrame = false;


	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Road Voronoi")
	void DrawDebugGraph() const;

protected:
	UPROPERTY(VisibleAnywhere)
	int Seed;
	
	FRandomStream RandomStream;
	FRoadNetworkData* Network;
	
	TArray<TTuple<FVector, FVector>> VoronoiEdges;
	TArray<int32> VoronoiCellMember;
	FBox Bounds;
	TArray<FVector> VoronoiSites;

	FBuildingPlotsCollection PlotsCollection;

	TArray<FVector> GenerateSites() const;

	FRoadNetworkData* CreateNetworkDataFromVoronoiGraph() const;
		
};
