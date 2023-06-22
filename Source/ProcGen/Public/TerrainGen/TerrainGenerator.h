// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainGenerator.generated.h"

UCLASS()
class PROCGEN_API ATerrainGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrainGenerator();
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=1))
	float VertexDistance = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float ZMultiplier = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* HeightCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0))
	float UVScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Material;
};
