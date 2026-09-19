


#pragma once
#include <vector>
#include <stdexcept>
#include "RoadGraphTypes.hpp"
#include <optional>



class VehiclePath {

	int id;
	bool invalidated;

	std::size_t currentRoadIndex;
	std::vector<RoadEdgeDescriptor> roadSegments;

public:
	
	VehiclePath(int id, const std::vector<RoadEdgeDescriptor> &roadSegments)
		: id(id), invalidated(false), currentRoadIndex(0) {
	
		if (roadSegments.size() <= 1)
			throw std::invalid_argument("Vehicle path cannot be empty or only a single road segment");

		this->roadSegments = roadSegments;
	}

	std::optional<RoadEdgeDescriptor> getPreviousEdge() const {

		if (currentRoadIndex == 0)
			return std::nullopt;

		return roadSegments[currentRoadIndex - 1];
	}
	
	std::optional<RoadEdgeDescriptor> getCurrentEdge() const {
		
		if (completed())
			return std::nullopt;

		return roadSegments[currentRoadIndex];
	}

	std::optional<RoadEdgeDescriptor> getNextEdge() const {
		
		if (completed() || currentRoadIndex + 1 >= roadSegments.size())
			return std::nullopt;

		return roadSegments[currentRoadIndex + 1];
	}

	void advanceEdge() { ++currentRoadIndex; }

	void invalidate() { invalidated = true; }

	bool completed() const { return currentRoadIndex >= roadSegments.size(); }
	int getId() const { return id; }
	bool isInvalidated() const { return invalidated; }
	std::vector<RoadEdgeDescriptor> getPath() const { return roadSegments; }
};


