// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PerlinNoiseGenerator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCGEN_API UPerlinNoiseGenerator : public UActorComponent
{
	GENERATED_BODY()

	const float Min_Noise_Scale = 0.000001f;

public:	
	UPerlinNoiseGenerator();

	UPROPERTY(EditAnywhere, meta=(ClampMin=0.0001))
	float NoiseScale = 0.1f;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin=0))
	int Octaves = 4;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin=0, ClampMax=1))
	float Persistence = 0.5f;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin=1))
	float Lacunarity = 4;


	UFUNCTION(BlueprintPure, Category="Procedural Generation|Noise")
	TArray<float> PerlinNoiseHeightmap(const int Width, const int Height);

	UFUNCTION(BlueprintCallable, Category="Procedural Generation|Noise")
	void SetSeed(int NewSeed);

protected:
	UPROPERTY(VisibleAnywhere)
	int XSize;
	UPROPERTY(VisibleAnywhere)
	int YSize;
	UPROPERTY(VisibleAnywhere)
	int Seed = 0;

	FRandomStream RandStream;
};
