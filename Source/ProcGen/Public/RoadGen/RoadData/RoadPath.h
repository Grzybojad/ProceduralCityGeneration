#pragma once
#include "RoadSegment.h"

class FRoadPath
{
public:
	void AddSegment(URoadSegment* RoadSegment);
	void SegmentWasSplit(URoadSegment* OldSegment, URoadSegment* NewSegment);

	TArray<URoadSegment*> GetSegments() const;
	
	URoadSegment* GetLastSegment() const;

	int Length() const;
	
protected:
	TArray<URoadSegment*> Path;
};
