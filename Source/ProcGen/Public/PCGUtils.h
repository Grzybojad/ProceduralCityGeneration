// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoadGen/RoadData/RoadSegmentToPlace.h"

class PROCGEN_API FPcgUtils
{
public:
	static bool RoadSegmentIntersection(const FRoadSegmentToPlace* SegmentToPlace, const URoadSegment* RoadSegment, FVector2D& OutIntersection);
	static bool LineSegmentToLineSegmentIntersection(const FVector2D& FromA, const FVector2D& FromB, const FVector2D& ToA, const FVector2D& ToB, FVector2D& OutIntersection);
	static bool LineToLineIntersection(const FVector2D& FromA, const FVector2D& FromB, const FVector2D& ToA, const FVector2D& ToB, FVector2D& OutIntersection);

	static float PointToRoadSegmentDistance(const URoadSegment* RoadSegment, const FVector2d Point);
	static float PointToLineSegmentDistance(const FVector2d Start, const FVector2d End, const FVector2d Point);
	
	static FVector FVector2DToFVector(const FVector2D V);
	static FVector2D FVectorToFVector2D(const FVector V);

	static float DotBetweenNodes(const FRoadNode* Node1, const FRoadNode* Node2, const FRoadNode* MiddleNode);
	static float AngleBetweenNodes(const FRoadNode* Node1, const FRoadNode* Node2, const FRoadNode* MiddleNode);
	static float AngleCCWBetweenNodes(const FRoadNode* Node1, const FRoadNode* Node2, const FRoadNode* MiddleNode);
	static float AngleCCWBetweenPositions(const FVector2D Pos1, const FVector2D Pos2, const FVector2D MiddlePos);

	static int PositiveModulo(const int I, const int N);
	
	static FVector CalculateCentroidIgnoringZ(const TArray<FVector> Points);

	static TArray<FVector2D> ShrinkPolygon(const TArray<FVector2D>& Vertices, const float Inset, const bool MergePotentialOverlaps = false);
	
	static TArray<FVector> ShrinkPolygon(const TArray<FVector>& Vertices, const float Inset, const bool MergePotentialOverlaps = false);
	
	static bool AreAllEdgesAboveLimit(const TArray<FVector>& Vertices, const float Limit);
};
