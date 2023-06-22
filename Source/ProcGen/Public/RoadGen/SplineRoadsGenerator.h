// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "RoadData/RoadPathsNodePositions.h"
#include "SplineRoadsGenerator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCGEN_API USplineRoadsGenerator : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Roads Generator")
	UStaticMesh* RoadMesh;
	
	UPROPERTY(EditAnywhere, Category="Roads Generator")
	UMaterial* RoadMaterial;

	UPROPERTY(EditAnywhere, Category="Roads Generator")
	float RoadMeshWidth;

	UPROPERTY(VisibleAnywhere, Category="Roads Generator")
	TArray<TWeakObjectPtr<USplineComponent>> SplineComponents;

	UPROPERTY(VisibleAnywhere, Category="Roads Generator")
	TArray<TWeakObjectPtr<USplineMeshComponent>> SplineMeshComponents;
	
	USplineRoadsGenerator();

	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Road")
	void GenerateRoads(const FRoadPathsNodePositions RoadPathsNodePositions);
	
	
protected:
	USplineComponent* AddSplineComponent();
	USplineMeshComponent* AddSplineMeshComponent();
	
	void DestroyedSpawned();
	
	void GenerateRoadMesh(const USplineComponent* Spline);

	void AdjustSplineToTerrain(FVector& StartPosition, FVector& StartTangent, FVector& EndPosition, FVector& EndTangent) const;
	void AdjustRoadPointToTerrain(FVector& Position, FVector& Tangent) const;

	FVector SubtractActorPosition(const FVector& Position) const;
};
