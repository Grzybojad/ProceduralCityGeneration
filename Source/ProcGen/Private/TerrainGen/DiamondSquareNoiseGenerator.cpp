// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainGen/DiamondSquareNoiseGenerator.h"

#include "Kismet/KismetMathLibrary.h"

UDiamondSquareNoiseGenerator::UDiamondSquareNoiseGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TArray<float> UDiamondSquareNoiseGenerator::DiamondSquareHeightmap(const int Size)
{
	HeightmapSize = Size+1;

	// Check if Size is a power of 2
	if((Size & (Size - 1)) != 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,TEXT("Diamond square doesn't work with sizes that aren't power of 2"));
	}
	
	Heightmap.Init(0, HeightmapSize * HeightmapSize);
	RandomStream = FRandomStream(Seed);
	
	int StepSize = HeightmapSize-1;
	float Scale = Roughness * StepSize;

	SetValue(0, 0, RandomStream.FRandRange(0, 1));
	SetValue(0, StepSize, RandomStream.FRandRange(0, 1));
	SetValue(StepSize, 0, RandomStream.FRandRange(0, 1));
	SetValue(StepSize, StepSize, RandomStream.FRandRange(0, 1));
	
	while(StepSize > 1)
	{
		DiamondStep(StepSize, Scale);
		SquareStep(StepSize, Scale);

		Scale *= Roughness;
		StepSize /= 2;
	}

	Normalize();

	return Heightmap;
}

void UDiamondSquareNoiseGenerator::SetSeed(const int NewSeed)
{
	Seed = NewSeed;
	RandomStream = FRandomStream(Seed);
}

void UDiamondSquareNoiseGenerator::DiamondStep(const int StepSize, const float Scale)
{
	const int HalfStep = StepSize / 2;
	
	for (int X = HalfStep; X < HeightmapSize; X += StepSize)
	{
		for (int Y = HalfStep; Y < HeightmapSize; Y += StepSize)
		{
			const float Average = GetDiamondStepAverage(X, Y, HalfStep);
			const float Offset = RandomStream.FRandRange(-Scale, Scale);
			SetValue(X, Y, Average + Offset);
		}
	}
}

void UDiamondSquareNoiseGenerator::SquareStep(const int StepSize, const float Scale)
{
	const int HalfStep = StepSize / 2;
	
	for (int X = 0; X < HeightmapSize; X += HalfStep)
	{
		for (int Y = (X + HalfStep) % StepSize; Y < HeightmapSize; Y += StepSize)
		{
			const float Average = GetSquareStepAverage(X, Y, HalfStep);
			const float Offset = RandomStream.FRandRange(-Scale, Scale);
			SetValue(X, Y, Average + Offset);
		}
	}
}

float UDiamondSquareNoiseGenerator::GetDiamondStepAverage(const int X, const int Y, const float HalfStep) const
{
	const float Sum = GetValue(X - HalfStep, Y - HalfStep) +
					  GetValue(X + HalfStep, Y - HalfStep) +
					  GetValue(X - HalfStep, Y + HalfStep) +
					  GetValue(X + HalfStep, Y + HalfStep);
	const float Average = Sum / 4.0f;
	return Average;
}

float UDiamondSquareNoiseGenerator::GetSquareStepAverage(const int X, const int Y, const float HalfStep) const
{
	float Sum = 0;
	int Count = 0;
		
	if (X >= HalfStep)
	{
		Sum += GetValue(X - HalfStep, Y);
		Count++;
	}
	if (X + HalfStep < HeightmapSize)
	{
		Sum += GetValue(X + HalfStep, Y);
		Count++;
	}
	if (Y >= HalfStep)
	{
		Sum += GetValue(X, Y - HalfStep);
		Count++;
	}
	if (Y + HalfStep < HeightmapSize)
	{
		Sum += GetValue(X, Y + HalfStep);
		Count++;
	}

	const float Average = Sum / Count;
		
	return Average;
}

int UDiamondSquareNoiseGenerator::GetIndex(int X, int Y) const
{
	X = FMath::Clamp(X, 0, HeightmapSize-1);
	Y = FMath::Clamp(Y, 0, HeightmapSize-1);
	
	return (Y * HeightmapSize) + X;
}

void UDiamondSquareNoiseGenerator::SetValue(const int X, const int Y, const float Value)
{
	Heightmap[GetIndex(X, Y)] = Value;
}

float UDiamondSquareNoiseGenerator::GetValue(const int X, const int Y) const
{
	return Heightmap[GetIndex(X, Y)];
}

void UDiamondSquareNoiseGenerator::Normalize()
{
	const float Min = FMath::Min(Heightmap);
	const float Max = FMath::Max(Heightmap);
	
	for (int X = 0; X < HeightmapSize; ++X)
	{
		for(int Y = 0; Y < HeightmapSize; ++Y)
		{
			const float OldValue = GetValue(X, Y);
			const float NewValue = UKismetMathLibrary::NormalizeToRange(OldValue, Min, Max);
			SetValue(X, Y, NewValue);
		}
	}
}

