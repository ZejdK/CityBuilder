


#pragma once
#include <string>
#include "SFML/System/Vector2.hpp"



class VehicleSourceSink {

	std::string label;
	bool active = false;
	float period;
	float lastUpdate = 0.f;
	std::string vehicleColour;

	const RoadJunctionGeometry* source;
	const RoadJunctionGeometry* sink;

	int pathId;

public:

	VehicleSourceSink(const std::string& label, float period, int pathId, const std::string &vehColour, sf::Vector2f sourcePos, sf::Vector2f sinkPos)
		: label(label), period(period), pathId(pathId), vehicleColour(vehColour), sourcePos(sourcePos), sinkPos(sinkPos) {}

	// time is in seconds
	void update(float time) { lastUpdate = time; }
	void setActive(bool active, float time) { this->active = active; this->lastUpdate = time; }
	void setPeriod(float period) { this->period = period; }

	bool isActive() const { return active; }
	int getPathId() const { return pathId; }

	float getPeriod() const { return period; }
	float getLastUpdate() const { return lastUpdate; }

	std::string getLabel() const { return label; }
	std::string getVehicleColour() const { return vehicleColour; }

	const RoadJunctionGeometry* getSourceJunction() const { return source; }
	const RoadJunctionGeometry* getSinkJunction() const { return sink; }
};


