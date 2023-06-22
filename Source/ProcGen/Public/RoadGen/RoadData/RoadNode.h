// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoadNode.generated.h"

UENUM()
enum ENodeState
{
	Disconnected,
	DeadEnd,
	Extension,
	Crossing
};

USTRUCT(BlueprintType)
struct FRoadNode
{
	GENERATED_BODY()

	FRoadNode();
	FRoadNode(int32 Index, FVector2D Position, TEnumAsByte<ENodeState> State);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Position;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ENodeState> State;

	TArray<FRoadNode*> ConnectedNodes;

	bool IsConnectedTo(const FRoadNode* Node) const;

	int GetNumberOfConnectedNodes() const;

	int GetMaxNewConnections(const int ConnectionsLimit) const;

	TArray<FRoadNode*> GetConnectedInDirection(FVector2D Direction, float MaxAngle);

	FRoadNode* GetRightMostNeighbour(const FVector2D ReferenceBack, const FRoadNode* ToIgnore);
	
	FColor GetColorByState() const;
};
