// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingGenerator.h"


UBuildingGenerator::UBuildingGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
	RandomStream = FRandomStream(Seed);
}

void UBuildingGenerator::SetSeed(const int NewSeed)
{
	Seed = NewSeed;
	RandomStream = FRandomStream(NewSeed);
}

void UBuildingGenerator::GenerateBuildingMeshesFromBuildingPlots(const FBuildingPlotsCollection BuildingPlots)
{
	DestroySpawnedMeshes();
	
	for (const FBuildingPlot BuildingPlot : BuildingPlots.BuildingPlots)
	{
		const float RandomHeight = RandomStream.FRandRange(MinHeight, MaxHeight);
		UProceduralMeshComponent* ProceduralMesh = AddBuildingMesh();

		TArray<FVector> WallNormals;
		TArray<FProcMeshTangent> WallTangents;
		TArray<FVector> RoofNormals;
		TArray<FProcMeshTangent> RoofTangents;
		GenerateBuildingMeshFromOutline(ProceduralMesh, BuildingPlot.Outline, RandomHeight);
	}
}

UProceduralMeshComponent* UBuildingGenerator::AddBuildingMesh()
{
	UProceduralMeshComponent* NewMeshComponent = NewObject<UProceduralMeshComponent>(this);
	NewMeshComponent->OnComponentCreated();
	NewMeshComponent->RegisterComponent();
	if (NewMeshComponent->bWantsInitializeComponent) NewMeshComponent->InitializeComponent();
	
	BuildingMeshes.Add(NewMeshComponent);
	
	return NewMeshComponent;
}

void UBuildingGenerator::DestroySpawnedMeshes()
{
	for (auto& ProceduralMeshComponentPtr : BuildingMeshes)
	{
		if (ProceduralMeshComponentPtr.IsValid())
		{
			UProceduralMeshComponent* ProceduralMeshComponent = ProceduralMeshComponentPtr.Get();
			ProceduralMeshComponent->DestroyComponent();
		}
	}
	BuildingMeshes.Empty();
}

void UBuildingGenerator::GenerateWallVertices(const TArray<FVector>& PlotOutline, const float Height, const float GroundHeight,
                                              TArray<FVector>& WallVertices, TArray<FVector2D>& WallUV0, const float UvScale)
{
	const int OutlinePointsCount = PlotOutline.Num();
	const int NumberOfWallVertices = OutlinePointsCount*4;
	WallVertices.SetNum(NumberOfWallVertices);
	WallUV0.SetNum(NumberOfWallVertices);

	float OutlineLength = 0;
	for (auto Line : PlotOutline) OutlineLength += Line.Length();
	float LastUvX = 0;
	float t = 0;
	const float UvY = Height * 0.0005 * UvScale;

	const int VerticesPerOutline = OutlinePointsCount*2;
	
	// Every wall vertex will be doubled, so that each wall has its own
	for (int i = 0; i < OutlinePointsCount; ++i)
	{
		const int OutlineIndex = i % OutlinePointsCount;

		const int VertexIndex1 = FPcgUtils::PositiveModulo(i*2 - 1, VerticesPerOutline);
		const int VertexIndex2 = i*2;
		
		// Bottom outline
		FVector BottomOutlinePos = PlotOutline[OutlineIndex];
		BottomOutlinePos.Z = GroundHeight;
		WallVertices[VertexIndex1] = BottomOutlinePos;
		WallVertices[VertexIndex2] = BottomOutlinePos;
		WallUV0[VertexIndex1] = FVector2D(LastUvX, 0);
		WallUV0[VertexIndex2] = FVector2D(LastUvX, 0);

		// Top outline
		const FVector TopOutlinePos = FVector(BottomOutlinePos.X, BottomOutlinePos.Y, GroundHeight+Height);
		WallVertices[VertexIndex1+VerticesPerOutline] = TopOutlinePos;
		WallVertices[VertexIndex2+VerticesPerOutline] = TopOutlinePos;
		WallUV0[VertexIndex1+VerticesPerOutline] = FVector2D(LastUvX, UvY);
		WallUV0[VertexIndex2+VerticesPerOutline] = FVector2D(LastUvX, UvY);
		
		const int NextOutlineIndex = (i+1) % OutlinePointsCount;
		const float LineLength = FVector2D::Distance(
			FPcgUtils::FVectorToFVector2D(PlotOutline[OutlineIndex]),
			FPcgUtils::FVectorToFVector2D(PlotOutline[NextOutlineIndex]));
		
		t += LineLength / OutlineLength;
		LastUvX = t * OutlineLength * 0.0005 * UvScale;
	}
}

void UBuildingGenerator::GenerateRoofVertices(const TArray<FVector>& PlotOutline, const float Height,
	TArray<FVector>& RoofVertices, TArray<FVector2D>& RoofUV0, const float UvScale)
{
	const int OutlinePointsCount = PlotOutline.Num();
	const int NumberOfRoofVertices = OutlinePointsCount;
	RoofVertices.SetNum(NumberOfRoofVertices);
	RoofUV0.SetNum(NumberOfRoofVertices);

	float MinX = PlotOutline[0].X;
	float MinY = PlotOutline[0].Y;
	float MaxX = PlotOutline[0].X;
	float MaxY = PlotOutline[0].Y;
	for (const FVector Point : PlotOutline)
	{
		if(Point.X > MaxX) MaxX = Point.X;
		if(Point.Y > MaxY) MaxY = Point.Y;
		if(Point.X < MinX) MinX = Point.X;
		if(Point.Y < MinY) MinY = Point.Y;
	}

	for (int i = 0; i < OutlinePointsCount; ++i)
	{
		const int OutlineIndex = i;
		const FVector OutlinePos = PlotOutline[OutlineIndex];
		const FVector RoofVertexPos = FVector(OutlinePos.X, OutlinePos.Y, Height);
		RoofVertices[i] = RoofVertexPos;
		const float RoofUvX = (PlotOutline[OutlineIndex].X - MinX ) / (MaxX - MinX) * UvScale;
		const float RoofUvY = (PlotOutline[OutlineIndex].Y - MinY ) / (MaxY - MinY) * UvScale;
		RoofUV0[i] = FVector2D(RoofUvX, RoofUvY);
	}
}

void UBuildingGenerator::GenerateWallTriangles(const TArray<FVector>& WallVertices, TArray<int32>& WallTriangles)
{
	const int VerticesPerOutline = WallVertices.Num() / 2;
	const int NumberOfFaces = VerticesPerOutline/2;
	
	for(int FaceIndex = 0; FaceIndex < NumberOfFaces; ++FaceIndex)
	{
		const int BottomLeft =	FaceIndex*2;
		const int BottomRight = FaceIndex*2 + 1;
		const int TopLeft =		FaceIndex*2 + VerticesPerOutline;
		const int TopRight =	FaceIndex*2 + VerticesPerOutline + 1;
		
		WallTriangles.Add(BottomLeft);
		WallTriangles.Add(BottomRight);
		WallTriangles.Add(TopRight);
		WallTriangles.Add(BottomLeft);
		WallTriangles.Add(TopRight);
		WallTriangles.Add(TopLeft);
	}
}

void UBuildingGenerator::GenerateRoofTriangles(const TArray<FVector>& RoofVertices, TArray<int32>& RoofTriangles)
{
	RoofTriangles = TriangulatePolygon(RoofVertices);
}

void UBuildingGenerator::GenerateWallSection(UProceduralMeshComponent* ProceduralMesh, const TArray<FVector>& PlotOutline, const float Height, const float GroundHeight,
	float WallUvScale, UMaterialInterface* WallMaterial, TArray<FVector>& WallVertices, TArray<int32>& WallTriangles,
	TArray<FVector2D>& WallUV0, TArray<FVector>& WallNormals, TArray<FProcMeshTangent>& WallTangents, const int MeshIndexOffset)
{
	GenerateWallVertices(PlotOutline, Height, GroundHeight, WallVertices, WallUV0, WallUvScale);
	GenerateWallTriangles(WallVertices, WallTriangles);
	GenerateMesh(ProceduralMesh, MeshIndexOffset, WallMaterial, WallVertices, WallTriangles, WallUV0, WallNormals, WallTangents);
}

void UBuildingGenerator::GenerateRoofSection(UProceduralMeshComponent* ProceduralMesh, const TArray<FVector>& PlotOutline, const float Height, float RoofUvScale,
	UMaterialInterface* RoofMaterial, TArray<FVector>& RoofVertices, TArray<int32>& RoofTriangles, TArray<FVector2D>& RoofUV0,
	TArray<FVector>& RoofNormals, TArray<FProcMeshTangent>& RoofTangents, const int MeshIndexOffset)
{
	GenerateRoofVertices(PlotOutline, Height,RoofVertices, RoofUV0, RoofUvScale);
	GenerateRoofTriangles(RoofVertices,  RoofTriangles);
	GenerateMesh(ProceduralMesh, MeshIndexOffset+1, RoofMaterial, RoofVertices, RoofTriangles, RoofUV0, RoofNormals, RoofTangents);
}

void UBuildingGenerator::GenerateMesh(UProceduralMeshComponent* ProceduralMesh, const int MeshIndex,
	UMaterialInterface* Material, const TArray<FVector>& Vertices, const TArray<int32>& Triangles,
	const TArray<FVector2D>& UV0, TArray<FVector>& Normals, TArray<FProcMeshTangent>& Tangents)
{
	TArray<FVector> GeneratedNormals = TArray<FVector>();
	TArray<FProcMeshTangent> GeneratedTangents = TArray<FProcMeshTangent>();

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, GeneratedNormals, GeneratedTangents);

	ProceduralMesh->CreateMeshSection(MeshIndex, Vertices, Triangles, GeneratedNormals, UV0, TArray<FColor>(), GeneratedTangents, true);
	ProceduralMesh->SetMaterial(MeshIndex, Material);
	
	Normals = GeneratedNormals;
	Tangents = GeneratedTangents;
}

void UBuildingGenerator::GenerateBuildingMeshFromOutline(UProceduralMeshComponent* ProceduralMesh, const TArray<FVector>& PlotOutline, const float Height) const
{
	const int Layers = RandomStream.FRandRange(1, MaxBuildingLayers);
	
	const float MaxHeightPerLayer = FMath::Clamp(Height / Layers, MinHeight, MaxHeight);
	
	TArray<FVector> LayerOutline = PlotOutline;
	float PreviousLayerRoofLevel = 0;
	float LayerRoofLevel = RandomStream.FRandRange(MinHeight, MaxHeightPerLayer);
	int LayerIndex = 0;
	
	while(LayerIndex < Layers)
	{
		TArray<FVector> WallVertices, RoofVertices;
		TArray<int> WallTriangles, RoofTriangles;
		TArray<FVector2D> WallUV0, RoofUV0;
		TArray<FVector> WallNormals, RoofNormals;
		TArray<FProcMeshTangent> WallTangents, RoofTangents;

		GenerateWallSection(ProceduralMesh, LayerOutline, LayerRoofLevel-PreviousLayerRoofLevel, PreviousLayerRoofLevel, WallUvScale, WallMaterial, WallVertices, WallTriangles, WallUV0, WallNormals, WallTangents, LayerIndex*2);
		GenerateRoofSection(ProceduralMesh, LayerOutline, LayerRoofLevel, RoofUvScale, RoofMaterial, RoofVertices, RoofTriangles, RoofUV0, RoofNormals, RoofTangents, LayerIndex*2);
		
		const float RandomHeight = RandomStream.FRandRange(MinHeight, MaxHeightPerLayer);
		PreviousLayerRoofLevel = LayerRoofLevel;
		LayerIndex++;
		LayerRoofLevel = LayerRoofLevel + RandomHeight;

		// Check if any of the edges isn't too short for shrinking
		if(FPcgUtils::AreAllEdgesAboveLimit(LayerOutline, LayerInset*2))
		{
			// Shrinking the layer outline
			// ShrinkPolygon expects the ends to match, so we have to do this weird bit of extra work
			FVector FirstElement = LayerOutline[0];
			LayerOutline.Add(FirstElement);
			LayerOutline = FPcgUtils::ShrinkPolygon(LayerOutline, LayerInset, true);
			LayerOutline.RemoveAt(LayerOutline.Num()-1);
		}
		else
		{
			break;
		}
	}
}

TArray<int32> UBuildingGenerator::TriangulatePolygon(const TArray<FVector>& Vertices)
{
	TArray<int32> Triangles;
	TArray<FVector> V = Vertices;
	
	while (V.Num() >= 3)
	{
		bool FoundEar = false;
		for (int i = 0; i < V.Num(); i++)
		{
			FVector Previous = V[(i + V.Num() - 1) % V.Num()];
			FVector Current = V[i];
			FVector Next = V[(i + 1) % V.Num()];
			
			if (IsEar(Previous, Current, Next, V))
			{
				Triangles.Add(Vertices.Find(Previous));
				Triangles.Add(Vertices.Find(Current));
				Triangles.Add(Vertices.Find(Next));
				V.Remove(Current);
				FoundEar = true;
				break;
			}
		}
		if (!FoundEar)
		{
			// Polygon cannot be triangulated
			return TArray<int32>();
		}
	}

	return Triangles;
}

bool UBuildingGenerator::IsPointInTriangle(const FVector& P, const FVector& A, const FVector& B, const FVector& C)
{
	const FVector V2V1 = B - A;
	const FVector V3V1 = C - A;
	const FVector Pv1 = P - A;
    
	const float Dot00 = FVector::DotProduct(V2V1, V2V1);
	const float Dot01 = FVector::DotProduct(V2V1, V3V1);
	const float Dot02 = FVector::DotProduct(V2V1, Pv1);
	const float Dot11 = FVector::DotProduct(V3V1, V3V1);
	const float Dot12 = FVector::DotProduct(V3V1, Pv1);
     
	const float InvDenom = 1.0f / (Dot00 * Dot11 - Dot01 * Dot01);
	const float U = (Dot11 * Dot02 - Dot01 * Dot12) * InvDenom;
	const float V = (Dot00 * Dot12 - Dot01 * Dot02) * InvDenom;
    
	return (U >= 0.0f) && (V >= 0.0f) && (U + V <= 1.0f);
}

bool UBuildingGenerator::IsEar(const FVector& Previous, const FVector& Current, const FVector& Next, const TArray<FVector>& Vertices)
{
	for (int i = 0; i < Vertices.Num(); i++)
	{
		FVector Vertex = Vertices[i];
		
		if( Vertex == Previous ||
			Vertex == Current ||
			Vertex == Next) continue;

		if(IsPointInTriangle(Vertex, Previous, Current, Next))
		{
			return false;
		}
	}

	return true;
}

