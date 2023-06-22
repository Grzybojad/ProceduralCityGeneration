// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TerrainVisualization.generated.h"

/**
 * 
 */
UCLASS()
class PROCGEN_API UTerrainVisualization : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Visualization")
	static UTexture2D* HeightmapToTexture(const TArray<float> HeightmapValues, const int XSize, const int YSize);
	
	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Visualization")
	static UTexture2D* HeightmapToInclineTexture(const TArray<float> HeightmapValues, const int XSize, const int YSize);

	static TArray<FVector2D> HeightmapToNormalsArray(const TArray<float> HeightmapValues, const int XSize, const int YSize);
};
