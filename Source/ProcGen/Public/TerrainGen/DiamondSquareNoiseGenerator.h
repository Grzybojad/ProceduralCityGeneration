// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DiamondSquareNoiseGenerator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCGEN_API UDiamondSquareNoiseGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDiamondSquareNoiseGenerator();

	UPROPERTY(EditAnywhere, meta=(ClampMin=0, ClampMax=1))
	float Roughness = 0.1;

	UFUNCTION(BlueprintPure, Category="Procedural Generation|Noise")
	TArray<float> DiamondSquareHeightmap(const int Size);

	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Noise")
	void SetSeed(int NewSeed);

protected:
	UPROPERTY(VisibleAnywhere)
	int HeightmapSize;
	UPROPERTY(VisibleAnywhere)
	int Seed = 0;
	
	TArray<float> Heightmap;
	FRandomStream RandomStream;

	void DiamondStep(const int StepSize, const float Scale);
	void SquareStep(const int StepSize, const float Scale);
	
	float GetDiamondStepAverage(const int X, const int Y, const float HalfStep) const;
	float GetSquareStepAverage(const int X, const int Y, const float StepSize) const;

	int GetIndex(int X, int Y) const;
	void SetValue(const int X, const int Y, const float Value);
	float GetValue(const int X, const int Y) const;

	void Normalize();
};
