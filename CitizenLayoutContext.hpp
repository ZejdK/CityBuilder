


#pragma once
#include <vector>
#include "RoadJunctionGeometry.hpp"
#include "RoadSegmentGeometry.hpp"



struct CitizenLayoutContext {

	const RoadSegmentGeometry* previousRoad;
	const RoadSegmentGeometry* road;
	const RoadSegmentGeometry* nextRoad;
	const RoadJunctionGeometry* incomingJunction;
	const RoadJunctionGeometry* outgoingJunction;
	std::vector<const RoadSegmentGeometry*> incomingJunctionRoads;
};


