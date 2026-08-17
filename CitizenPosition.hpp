


#pragma once
#include "RoadGraphTypes.hpp"



struct CitizenPosition {

	RoadVertexDescriptor from;
	RoadVertexDescriptor to;

	float s;

	CitizenPosition(RoadVertexDescriptor from, RoadVertexDescriptor to, float s)
		: from(from), to(to), s(s) {}
};


