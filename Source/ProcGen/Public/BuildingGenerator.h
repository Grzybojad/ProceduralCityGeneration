// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "PCGUtils.h"
#include "RoadGen/RoadData/BuildingPlotsCollection.h"
#include "BuildingGenerator.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROCGEN_API UBuildingGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuildingGenerator();

	UPROPERTY(EditAnywhere, Category="Building Generator")
	float MinHeight = 500;
	
	UPROPERTY(EditAnywhere, Category="Building Generator")
	float MaxHeight = 5000;

	UPROPERTY(EditAnywhere, Category="Building Generator")
	UMaterialInterface* WallMaterial;
	
	UPROPERTY(EditAnywhere, Category="Building Generator")
	UMaterialInterface* RoofMaterial;

	UPROPERTY(EditAnywhere, Category="Building Generator")
	float WallUvScale = 1;

	UPROPERTY(EditAnywhere, Category="Building Generator")
	float RoofUvScale = 1;

	UPROPERTY(EditAnywhere, Category="Building Generator", meta=(ClampMin=1, ClampMax=10))
	int MaxBuildingLayers = 3;

	UPROPERTY(EditAnywhere, Category="Building Generator", meta=(ClampMin=0))
	int LayerInset;
	
	UPROPERTY(VisibleAnywhere)
	TArray<TWeakObjectPtr<UProceduralMeshComponent>> BuildingMeshes;
	

	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Building Mesh")
	void SetSeed(const int NewSeed);

	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Building Mesh")
	void GenerateBuildingMeshFromOutline(UProceduralMeshComponent* ProceduralMesh, const TArray<FVector>& PlotOutline, const float Height) const;
	
	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Building Mesh")
	void GenerateBuildingMeshesFromBuildingPlots(const FBuildingPlotsCollection BuildingPlots);

	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Building Mesh")
	void DestroySpawnedMeshes();

protected:
	UPROPERTY(VisibleAnywhere)
	int Seed;
	FRandomStream RandomStream;
	
	UProceduralMeshComponent* AddBuildingMesh();
	
	static void GenerateWallVertices(const TArray<FVector>& PlotOutline, const float Height, const float GroundHeight,
		TArray<FVector>& WallVertices, TArray<FVector2D>& WallUV0, const float UvScale);

	static void GenerateRoofVertices(const TArray<FVector>& PlotOutline, const float Height,
		TArray<FVector>& RoofVertices, TArray<FVector2D>& RoofUV0, const float UvScale);

	static void GenerateWallTriangles(const TArray<FVector>& WallVertices, TArray<int32>& WallTriangles);

	static void GenerateRoofTriangles(const TArray<FVector>& RoofVertices, TArray<int32>& RoofTriangles);

	static void GenerateWallSection(UProceduralMeshComponent* ProceduralMesh, const TArray<FVector>& PlotOutline, const float Height, const float GroundHeight,
	                                float WallUvScale, UMaterialInterface* WallMaterial, TArray<FVector>& WallVertices, TArray<int32>& WallTriangles,
	                                TArray<FVector2D>& WallUV0, TArray<FVector>& WallNormals, TArray<FProcMeshTangent>& WallTangents, const int MeshIndexOffset = 0);
	
	static void GenerateRoofSection(UProceduralMeshComponent* ProceduralMesh, const TArray<FVector>& PlotOutline, const float Height,
	                                float RoofUvScale, UMaterialInterface* RoofMaterial, TArray<FVector>& RoofVertices, TArray<int32>& RoofTriangles, TArray
	                                <FVector2D>& RoofUV0, TArray<FVector>& RoofNormals, TArray<FProcMeshTangent>& RoofTangents, const int MeshIndexOffset = 0);

	static void GenerateMesh(UProceduralMeshComponent* ProceduralMesh, const int MeshIndex, UMaterialInterface* Material, 
		const TArray<FVector>& Vertices, const TArray<int32>& Triangles,
		const TArray<FVector2D>& UV0, TArray<FVector>& Normals, TArray<FProcMeshTangent>& Tangents);
	
	static TArray<int32> TriangulatePolygon(const TArray<FVector>& Vertices);

	static bool IsPointInTriangle(const FVector& P, const FVector& A, const FVector& B, const FVector& C);

	static bool IsEar(const FVector& Previous, const FVector& Current, const FVector& Next, const TArray<FVector>& Vertices);
};
