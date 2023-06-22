// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGen/TerrainMeshLibrary.h"

void UTerrainMeshLibrary::GenerateVertices(
		const TArray<float> HeightmapValues, const int XSize, const int YSize,
		const float VertexDistance, const float ZMultiplier,
		const float UVScale,
		TArray<FVector>& Vertices,
		TArray<FVector2D>& UV0
		)
{
	if(HeightmapValues.IsEmpty()) return;
	
	TArray<FVector> GeneratedVertices = TArray<FVector>();
	TArray<FVector2D> GeneratedUV0 = TArray<FVector2D>();
	
	for (int X = 0; X < XSize+1; ++X)
	{
		for(int Y = 0; Y < YSize+1; ++Y)
		{
			const int32 Index = X * (YSize+1) + Y;
			const float NoiseVal = HeightmapValues[Index];
			const float Z = NoiseVal * ZMultiplier;
			GeneratedVertices.Add(FVector(X * VertexDistance, Y * VertexDistance, Z));
			GeneratedUV0.Add(FVector2D(X * UVScale, Y * UVScale));
		}
	}

	Vertices = GeneratedVertices;
	UV0 = GeneratedUV0;
}

TArray<int32> UTerrainMeshLibrary::GenerateTriangles(const int XSize, const int YSize)
{
	TArray<int32> Triangles = TArray<int32>();
	
	for(int X = 0; X < XSize; ++X)
	{
		for (int Y = 0; Y < YSize; ++Y)
		{
			int Vertex = (X * (YSize+1)) + Y;
			
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);
		}
	}

	return Triangles;
}

TArray<float> UTerrainMeshLibrary::RemapHeightmapValues(const TArray<float> HeightmapValues,
	const int XSize, const int YSize, const UCurveFloat* HeightCurve)
{
	TArray<float> NewHeightmapValues;
	if(HeightmapValues.IsEmpty()) return NewHeightmapValues;
	
	NewHeightmapValues.SetNum(HeightmapValues.Num());
	
	for (int X = 0; X < XSize+1; ++X)
	{
		for(int Y = 0; Y < YSize+1; ++Y)
		{
			const int32 Index = X * (YSize+1) + Y;
			const float HeightmapValue = HeightmapValues[Index];
			const float NewZ = HeightCurve->GetFloatValue(HeightmapValue);
			NewHeightmapValues[Index] = NewZ;
		}
	}

	return NewHeightmapValues;
}

void UTerrainMeshLibrary::GenerateTerrainMesh(
		UProceduralMeshComponent* ProceduralMesh,
		UMaterialInterface* Material,
		const TArray<FVector> Vertices, const TArray<int32> Triangles, const TArray<FVector2D> UV0,
		TArray<FVector>& Normals,
		TArray<FProcMeshTangent>& Tangents
		)
{
	TArray<FVector> GeneratedNormals = TArray<FVector>();
	TArray<FProcMeshTangent> GeneratedTangents = TArray<FProcMeshTangent>();
	
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, GeneratedNormals, GeneratedTangents);
	
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, GeneratedNormals, UV0, TArray<FColor>(), GeneratedTangents, true);
	ProceduralMesh->SetMaterial(0, Material);

	Normals = GeneratedNormals;
	Tangents = GeneratedTangents;
}

void UTerrainMeshLibrary::HeightmapToMesh(
	const TArray<float> HeightmapValues, const int XSize, const int YSize,
	const float VertexDistance, const float ZMultiplier,
	const float UVScale,
	TArray<FVector>& Vertices,
	TArray<int32>& Triangles,
	TArray<FVector2D>& UV0
	)
{
	if(HeightmapValues.IsEmpty()) return;
	
	Triangles = GenerateTriangles(XSize, YSize);
	GenerateVertices(HeightmapValues, XSize, YSize, VertexDistance, ZMultiplier, UVScale, Vertices, UV0);
}

