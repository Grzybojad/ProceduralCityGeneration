#pragma once

#include "RoadNode.h"
#include "RoadSegment.generated.h"


UCLASS(BlueprintType)
class URoadSegment : public UObject
{
	GENERATED_BODY()

public:
	FRoadNode* Start;
	FRoadNode* End;
	
	void SetNodesConnection() const;

	URoadSegment* SplitSegment(const FVector2D SplitPoint, const int NewNodeIndex);

	FVector2D GetDirection() const;

protected:
	static void SetNodesConnection(FRoadNode* NodeA, FRoadNode* NodeB);
};