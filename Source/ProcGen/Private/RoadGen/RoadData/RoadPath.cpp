#include "RoadGen/RoadData/RoadPath.h"

void FRoadPath::AddSegment(URoadSegment* RoadSegment)
{
	Path.Add(RoadSegment);
}

void FRoadPath::SegmentWasSplit(URoadSegment* OldSegment, URoadSegment* NewSegment)
{
	const int Index = Path.Find(OldSegment);

	if(Index == Path.Num() - 1)
	{
		Path.Add(NewSegment);
	}
	else
	{
		Path.Insert(NewSegment, Index+1);
	}
}

TArray<URoadSegment*> FRoadPath::GetSegments() const
{
	return Path;
}

URoadSegment* FRoadPath::GetLastSegment() const
{
	return Path.Last();
}

int FRoadPath::Length() const
{
	return Path.Num();
}
