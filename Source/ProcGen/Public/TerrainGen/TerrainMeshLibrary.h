// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "TerrainMeshLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROCGEN_API UTerrainMeshLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Procedural Generation|Mesh")
	static void GenerateVertices(
		const TArray<float> HeightmapValues, const int XSize, const int YSize,
		const float VertexDistance, const float ZMultiplier,
		const float UVScale,
		TArray<FVector>& Vertices,
		TArray<FVector2D>& UV0
		);

	UFUNCTION(BlueprintPure, Category="Procedural Generation|Mesh")
	static TArray<int32> GenerateTriangles(const int XSize, const int YSize);

	UFUNCTION(BlueprintPure, Category="Procedural Generation|Mesh")
	static TArray<float> RemapHeightmapValues(const TArray<float> HeightmapValues,
		const int XSize, const int YSize, const UCurveFloat* HeightCurve);
	
	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Mesh")
	static void GenerateTerrainMesh(
		UProceduralMeshComponent* ProceduralMesh,
		UMaterialInterface* Material,
		const TArray<FVector> Vertices, const TArray<int32> Triangles, const TArray<FVector2D> UV0,
		TArray<FVector>& Normals,
		TArray<FProcMeshTangent>& Tangents
		);

	UFUNCTION(BlueprintPure, Category="Procedural Generation|Mesh")
	static void HeightmapToMesh(
	const TArray<float> HeightmapValues, const int XSize, const int YSize,
	const float VertexDistance, const float ZMultiplier,
	const float UVScale,
	TArray<FVector>& Vertices,
	TArray<int32>& Triangles,
	TArray<FVector2D>& UV0
		);
	
};
