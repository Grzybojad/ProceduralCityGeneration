// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinNoiseGenerator.h"

UPerlinNoiseGenerator::UPerlinNoiseGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPerlinNoiseGenerator::SetSeed(const int NewSeed)
{
	Seed = NewSeed;
	RandStream = FRandomStream(Seed);
}

TArray<float> UPerlinNoiseGenerator::PerlinNoiseHeightmap(const int Width, const int Height)
{
	XSize = Width;
	YSize = Height;
	
	TArray<float> HeightmapValues = TArray<float>();
	const float Scale = NoiseScale > 0 ? NoiseScale : Min_Noise_Scale;

	RandStream = FRandomStream(Seed);
	TArray<FVector2D> OctaveOffsets = TArray<FVector2D>();
	for (int Octave = 0; Octave < Octaves; ++Octave)
	{
		constexpr int Offset_Range = 10000;
		const float OffsetX = RandStream.RandRange(-Offset_Range, Offset_Range);
		const float OffsetY = RandStream.RandRange(-Offset_Range, Offset_Range);
		OctaveOffsets.Add(FVector2D(OffsetX, OffsetY));
	}
	
	float MaxNoiseHeight = -FLT_MAX;
	float MinNoiseHeight = FLT_MAX;

	const FVector2D HalfSize = FVector2D(XSize/2, YSize/2);
	
	for (int X = 0; X < XSize+1; ++X)
	{
		for(int Y = 0; Y < YSize+1; ++Y)
		{
			float Amplitude = 1;
			float Frequency = 1;
			float NoiseHeight = 0;

			for (int Octave = 0; Octave < Octaves; ++Octave)
			{
				const FVector2D SamplePos = (FVector2D(X, Y) - HalfSize) * Scale * Frequency + OctaveOffsets[Octave];
				const float PerlinValue = FMath::PerlinNoise2D(SamplePos);
				NoiseHeight += PerlinValue * Amplitude;

				Amplitude *= Persistence;
				Frequency *= Lacunarity;
			}

			if(NoiseHeight > MaxNoiseHeight) MaxNoiseHeight = NoiseHeight;
			if(NoiseHeight < MinNoiseHeight) MinNoiseHeight = NoiseHeight;
			
			HeightmapValues.Add(NoiseHeight);
		}
	}

	// Normalize heightmap values
	for (int X = 0; X < XSize+1; ++X)
	{
		for(int Y = 0; Y < YSize+1; ++Y)
		{
			const float Val = HeightmapValues[X*(YSize+1) + Y];
			HeightmapValues[X*(YSize+1) + Y] = UKismetMathLibrary::NormalizeToRange(Val, MinNoiseHeight, MaxNoiseHeight);
		}
	}
	
	return HeightmapValues;
}

