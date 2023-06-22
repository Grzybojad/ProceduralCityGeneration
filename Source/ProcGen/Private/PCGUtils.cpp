// Fill out your copyright notice in the Description page of Project Settings.


#include "PCGUtils.h"

bool FPcgUtils::RoadSegmentIntersection(const FRoadSegmentToPlace* SegmentToPlace, const URoadSegment* RoadSegment,
	FVector2D& OutIntersection)
{
	const FVector2D FromA = RoadSegment->Start->Position;
	const FVector2D ToA = RoadSegment->End->Position;
	const FVector2D FromB = SegmentToPlace->Start->Position;
	const FVector2D ToB = SegmentToPlace->Target;

	if(FromA == FromB || ToA == ToB || FromA == ToB || FromB == ToA) return false;

	return LineSegmentToLineSegmentIntersection(FromA, FromB,ToA, ToB,OutIntersection);
}

bool FPcgUtils::LineSegmentToLineSegmentIntersection(const FVector2D& FromA, const FVector2D& FromB, const FVector2D& ToA, const FVector2D& ToB, FVector2D& OutIntersection)
{
	const float P0_X = FromA.X;
	const float P0_Y = FromA.Y;
	const float P1_X = ToA.X;
	const float P1_Y = ToA.Y;

	const float P2_X = FromB.X;
	const float P2_Y = FromB.Y;
	const float P3_X = ToB.X;
	const float P3_Y = ToB.Y;

	const float S1_X = P1_X - P0_X;
	const float S1_Y = P1_Y - P0_Y;
	const float S2_X = P3_X - P2_X;
	const float S2_Y = P3_Y - P2_Y;

	const float SDiv = -S2_X * S1_Y + S1_X * S2_Y;
	const float TDiv = -S2_X * S1_Y + S1_X * S2_Y;

	// If parallel
	if(SDiv == 0 || TDiv == 0) return false;
	
	const float S = (-S1_Y * (P0_X - P2_X) + S1_X * (P0_Y - P2_Y)) / SDiv;
	const float T = (S2_X * (P0_Y - P2_Y) - S2_Y * (P0_X - P2_X)) / TDiv;

	if (S >= 0 && S <= 1 && T >= 0 && T <= 1)
	{
		// Collision detected
		OutIntersection.X = P0_X + (T * S1_X);
		OutIntersection.Y = P0_Y + (T * S1_Y);
		return true;
	}

	return false; // No collision
}

bool FPcgUtils::LineToLineIntersection(const FVector2D& FromA, const FVector2D& FromB, const FVector2D& ToA,
	const FVector2D& ToB, FVector2D& OutIntersection)
{
	const float P0_X = FromA.X;
	const float P0_Y = FromA.Y;
	const float P1_X = ToA.X;
	const float P1_Y = ToA.Y;

	const float P2_X = FromB.X;
	const float P2_Y = FromB.Y;
	const float P3_X = ToB.X;
	const float P3_Y = ToB.Y;

	const float S1_X = P1_X - P0_X;
	const float S1_Y = P1_Y - P0_Y;
	const float S2_X = P3_X - P2_X;
	const float S2_Y = P3_Y - P2_Y;
	
	const float SDiv = -S2_X * S1_Y + S1_X * S2_Y;
	const float TDiv = -S2_X * S1_Y + S1_X * S2_Y;

	// If parallel
	if(TDiv == 0) return false;
	
	const float T = (S2_X * (P0_Y - P2_Y) - S2_Y * (P0_X - P2_X)) / TDiv;

	OutIntersection.X = P0_X + (T * S1_X);
	OutIntersection.Y = P0_Y + (T * S1_Y);
	return true;
}

float FPcgUtils::PointToRoadSegmentDistance(const URoadSegment* RoadSegment, const FVector2d Point)
{
	return PointToLineSegmentDistance(RoadSegment->Start->Position, RoadSegment->End->Position, Point);
}


// Return minimum distance between line segment Start-End and Point
// Adapted https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
float FPcgUtils::PointToLineSegmentDistance(const FVector2d Start, const FVector2d End, const FVector2d Point)
{
	const float SquaredLength = (End-Start).SquaredLength();   // i.e. |w-v|^2 -  avoid a sqrt
	if (SquaredLength == 0)
	{
		// Start == End case
		return FVector2D::Distance(Point, Start); 
	}
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	// We clamp t from [0,1] to handle points outside the segment vw.
	const float T = FMath::Max(0, FMath::Min(1, FVector2D::DotProduct(Point - Start, End - Start) / SquaredLength));
	const FVector2D Projection = Start + T * (End - Start);  // Projection falls on the segment
	return FVector2D::Distance(Point, Projection);
}


FVector FPcgUtils::FVector2DToFVector(const FVector2D V)
{
	return FVector(V.X, V.Y, 0);
}

FVector2D FPcgUtils::FVectorToFVector2D(const FVector V)
{
	return FVector2D(V.X, V.Y);
}

float FPcgUtils::DotBetweenNodes(const FRoadNode* Node1, const FRoadNode* Node2, const FRoadNode* MiddleNode)
{
	const FVector2d Node1Dir = (Node1->Position - MiddleNode->Position).GetSafeNormal();
	const FVector2d Node2Dir = (Node2->Position - MiddleNode->Position).GetSafeNormal();
	const float Dot = FVector2D::DotProduct(Node1Dir, Node2Dir);
	return Dot;
}

float FPcgUtils::AngleBetweenNodes(const FRoadNode* Node1, const FRoadNode* Node2, const FRoadNode* MiddleNode)
{
	const FVector2d Node1Dir = (Node1->Position - MiddleNode->Position).GetSafeNormal();
	const FVector2d Node2Dir = (Node2->Position - MiddleNode->Position).GetSafeNormal();
	const float Dot = FVector2D::DotProduct(Node1Dir, Node2Dir);
	const float Angle = FMath::Acos(Dot) / PI * 180;
	return Angle;
}

float FPcgUtils::AngleCCWBetweenNodes(const FRoadNode* Node1, const FRoadNode* Node2, const FRoadNode* MiddleNode)
{
	return AngleCCWBetweenPositions(Node1->Position, Node2->Position, MiddleNode->Position);
}

float FPcgUtils::AngleCCWBetweenPositions(const FVector2D Pos1, const FVector2D Pos2, const FVector2D MiddlePos)
{
	const FVector2d Node1Dir = (Pos1 - MiddlePos).GetSafeNormal();
	const FVector2d Node2Dir = (Pos2 - MiddlePos).GetSafeNormal();
	const float Dot = FVector2D::DotProduct(Node1Dir, Node2Dir);
	float Angle = FMath::Acos(Dot) / PI * 180;

	// Based on https://stackoverflow.com/questions/13221873/determining-if-one-2d-vector-is-to-the-right-or-left-of-another
	const FVector2D Node2DirRotated90CCW = Node2Dir.GetRotated(-90);
	const bool OnRight = FVector2D::DotProduct(Node1Dir, Node2DirRotated90CCW) < 0;
	if(OnRight)
	{
		Angle = 360 - Angle;
	}
	
	return Angle;
}

int FPcgUtils::PositiveModulo(const int I, const int N)
{
	return (I % N + N) % N;
}

// From https://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
FVector FPcgUtils::CalculateCentroidIgnoringZ(const TArray<FVector> Points)
{
	FVector Centroid = {0, 0, 0};
	double SignedArea = 0.0;

	// For all vertices
	for (int i=0; i<Points.Num(); ++i)
	{
		const float X0 = Points[i].X;
		const float Y0 = Points[i].Y;
		const float X1 = Points[(i+1) % Points.Num()].X;
		const float Y1 = Points[(i+1) % Points.Num()].Y;
		const float A = X0*Y1 - X1*Y0;
		SignedArea += A;
		Centroid.X += (X0 + X1)*A;
		Centroid.Y += (Y0 + Y1)*A;
	}

	SignedArea *= 0.5;
	Centroid.X /= (6.0*SignedArea);
	Centroid.Y /= (6.0*SignedArea);

	return Centroid;
}

// Expects the vertices to form a closed loop, meaning Vertices[0] == Vertices[Vertices.Num()-1]
TArray<FVector2D> FPcgUtils::ShrinkPolygon(const TArray<FVector2D>& Vertices, const float Inset, const bool MergePotentialOverlaps)
{
	const int VertexNum = Vertices.Num()-1;
	TArray<FVector2D> PlotPolygon;
	
	for (int VertexIndex = 0; VertexIndex < VertexNum+1; ++VertexIndex)
	{
		const int PreviousIndex = PositiveModulo(VertexIndex-1, VertexNum);
		const int NextIndex = PositiveModulo(VertexIndex+1, VertexNum);
		const FVector2D Previous = Vertices[PreviousIndex];
		const FVector2D Currents = Vertices[VertexIndex];
		const FVector2D Next = Vertices[NextIndex];

		const float Angle = AngleCCWBetweenPositions(Previous, Next, Currents);

		FVector2D Direction = (Previous - Currents).GetSafeNormal();
		Direction = Direction.GetRotated(Angle/2);
		FVector2D VertexPos = Currents;
		const float OffsetAngle = Angle / 2 / 180 * PI;
		FVector2D Offset = Direction * Inset / sin(OffsetAngle);
		FVector2D PlotVertexPos = VertexPos + Offset;

		PlotPolygon.Add(PlotVertexPos);
	}

	if(!MergePotentialOverlaps) return PlotPolygon;

	// Merge potentially overlapping vertices
	TArray<FVector2D> AdjustedPlotPolygon;
	for (int VertexIndex = 0; VertexIndex < PlotPolygon.Num(); ++VertexIndex)
	{
		const int NextIndex = PositiveModulo(VertexIndex+1, VertexNum);
		const FVector2D Current = Vertices[VertexIndex];
		const FVector2D Next = Vertices[NextIndex];
		
		const float DistanceToNext = FVector2D::Distance(Current, Next);
	
		if(DistanceToNext < Inset * 1.5)
		{
			// Move the current node to the middle point between it and the last one
			// and remove (skip) the next node
			const FVector2D Middle = (PlotPolygon[VertexIndex] + PlotPolygon[NextIndex]) / 2;
			AdjustedPlotPolygon.Add(Middle);
			VertexIndex++;
		}
		else
		{
			AdjustedPlotPolygon.Add(PlotPolygon[VertexIndex]);
		}
	}
	
	return AdjustedPlotPolygon;
}

TArray<FVector> FPcgUtils::ShrinkPolygon(const TArray<FVector>& Vertices, const float Inset,
	const bool MergePotentialOverlaps)
{
	const int VertexNum = Vertices.Num()-1;
	TArray<FVector> PlotPolygon;
	
	for (int VertexIndex = 0; VertexIndex < VertexNum+1; ++VertexIndex)
	{
		const int PreviousIndex = PositiveModulo(VertexIndex-1, VertexNum);
		const int NextIndex = PositiveModulo(VertexIndex+1, VertexNum);
		const FVector Previous = Vertices[PreviousIndex];
		const FVector Current = Vertices[VertexIndex];
		const FVector Next = Vertices[NextIndex];

		const float Angle = AngleCCWBetweenPositions(FVectorToFVector2D(Previous), FVectorToFVector2D(Next), FVectorToFVector2D(Current));

		FVector Direction = (Previous - Current).GetSafeNormal();
		Direction = Direction.RotateAngleAxis(Angle/2, FVector(0, 0, 1));
		FVector VertexPos = Current;
		const float OffsetAngle = Angle / 2 / 180 * PI;
		FVector Offset = Direction * Inset / sin(OffsetAngle);
		FVector PlotVertexPos = VertexPos + Offset;

		PlotPolygon.Add(PlotVertexPos);
	}

	if(!MergePotentialOverlaps) return PlotPolygon;

	// Merge potentially overlapping vertices
	TArray<FVector> AdjustedPlotPolygon;
	for (int VertexIndex = 0; VertexIndex < PlotPolygon.Num(); ++VertexIndex)
	{
		const int NextIndex = PositiveModulo(VertexIndex+1, VertexNum);
		const FVector Current = Vertices[VertexIndex];
		const FVector Next = Vertices[NextIndex];
		
		const float DistanceToNext = FVector::Distance(Current, Next);
	
		if(DistanceToNext < Inset * 1.5)
		{
			// Move the current node to the middle point between it and the last one
			// and remove (skip) the next node
			const FVector Middle = (PlotPolygon[VertexIndex] + PlotPolygon[NextIndex]) / 2;
			AdjustedPlotPolygon.Add(Middle);
			VertexIndex++;
		}
		else
		{
			AdjustedPlotPolygon.Add(PlotPolygon[VertexIndex]);
		}
	}
	
	return AdjustedPlotPolygon;
}

// Expects each vertex to be separate
bool FPcgUtils::AreAllEdgesAboveLimit(const TArray<FVector>& Vertices, const float Limit)
{
	for (int i=0; i<Vertices.Num()-1; ++i)
	{
		const float Distance = FVector::Distance(Vertices[i], Vertices[i+1]);

		if(Distance < Limit) return false;
	}

	const float ClosingDistance = FVector::Distance(Vertices[Vertices.Num()-1], Vertices[0]);

	if(ClosingDistance < Limit) return false;

	return true;
}
