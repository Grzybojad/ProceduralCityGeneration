// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGen/TerrainVisualization.h"

UTexture2D* UTerrainVisualization::HeightmapToTexture(const TArray<float> HeightmapValues, const int XSize, const int YSize)
{
	UTexture2D* HeightmapTexture = UTexture2D::CreateTransient(XSize+1, YSize+1);
	FTexture2DMipMap* MipMap = &HeightmapTexture->GetPlatformData()->Mips[0];
	FByteBulkData* ImageData = &MipMap->BulkData;
	uint8* Pixels = static_cast<uint8*>(ImageData->Lock(LOCK_READ_WRITE));

	const float MinHeight = FMath::Min(HeightmapValues);
	const float MaxHeight = FMath::Max(HeightmapValues);
	
	for (int32 X = 0; X < XSize+1; X++)
	{
		for (int32 Y = 0; Y < YSize+1; Y++)
		{
			const int32 CurHeightmapIndex = (X * (YSize+1)) + Y;
			const int32 CurPixelIndex = (Y * (XSize+1)) + X; // invert X-Y for the texture
			
			const float HeightmapValue = HeightmapValues[CurHeightmapIndex];
			const float NormalizedHeight = (HeightmapValue - MinHeight) / MaxHeight;
			
			Pixels[4 * CurPixelIndex] = NormalizedHeight * 255; // 0;	 // B
			Pixels[4 * CurPixelIndex + 1] = NormalizedHeight * 255; //(1-NormalizedHeight) * 255; // G
			Pixels[4 * CurPixelIndex + 2] = NormalizedHeight * 255; // R
			Pixels[4 * CurPixelIndex + 3] = 255; // A
		}
	}

	ImageData->Unlock();
	HeightmapTexture->UpdateResource();
	
	return HeightmapTexture;
}

UTexture2D* UTerrainVisualization::HeightmapToInclineTexture(const TArray<float> HeightmapValues, const int XSize, const int YSize)
{
	UTexture2D* HeightmapTexture = UTexture2D::CreateTransient(XSize+1, YSize+1);
	FTexture2DMipMap* MipMap = &HeightmapTexture->GetPlatformData()->Mips[0];
	FByteBulkData* ImageData = &MipMap->BulkData;
	uint8* Pixels = static_cast<uint8*>(ImageData->Lock(LOCK_READ_WRITE));

	TArray<FVector2D> Normals = HeightmapToNormalsArray(HeightmapValues, XSize, YSize);
	
	for (int32 X = 0; X < XSize+1; X++)
	{
		for (int32 Y = 0; Y < YSize+1; Y++)
		{
			const int32 CurHeightmapIndex = (X * (YSize+1)) + Y;
			const int32 CurPixelIndex = (Y * (XSize+1)) + X; // invert X-Y for the texture
			
			const FVector2D NormalValue = Normals[CurHeightmapIndex];
			const float Slope = FMath::Max(FMath::Abs(NormalValue.Y), FMath::Abs(NormalValue.X));
			
			Pixels[4 * CurPixelIndex] = 0;	 // B
			Pixels[4 * CurPixelIndex + 1] = (1-Slope) * 255; // G
			Pixels[4 * CurPixelIndex + 2] = Slope * 255; // R
			Pixels[4 * CurPixelIndex + 3] = 255; // A
		}
	}

	ImageData->Unlock();
	HeightmapTexture->UpdateResource();
	
	return HeightmapTexture;
}

TArray<FVector2D> UTerrainVisualization::HeightmapToNormalsArray(const TArray<float> HeightmapValues, const int XSize, const int YSize)
{
	const int32 TextureSize = HeightmapValues.Num();
	TArray<FVector2D> InclineArray;
	InclineArray.SetNum(TextureSize);

	float MaxDiff = 0;
	
	for (int32 X = 0; X < XSize+1; X++)
	{
		for (int32 Y = 0; Y < YSize+1; Y++)
		{
			const int32 Index = X*(YSize+1) + Y;
			float DiffX = 0, DiffY = 0;
			
			if(Index+1 < TextureSize)
			{
				DiffX = HeightmapValues[Index+1] - HeightmapValues[Index];
				
				if(FMath::Abs(DiffX) > MaxDiff) MaxDiff = FMath::Abs(DiffX);
			}
			if(Index+YSize < TextureSize)
			{
				DiffY = HeightmapValues[Index+YSize] - HeightmapValues[Index];
				
				if(FMath::Abs(DiffY) > MaxDiff) MaxDiff = FMath::Abs(DiffY);
			}
			
			InclineArray[Index] = FVector2D(DiffX, DiffY);
		}
	}

	// Normalize
	for (int32 X = 0; X < XSize+1; X++)
	{
		for (int32 Y = 0; Y < YSize+1; Y++)
		{
			const int32 Index = X*(YSize+1) + Y;
			
			InclineArray[Index] /= MaxDiff;
		}
	}

	return InclineArray;
}