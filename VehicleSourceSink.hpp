


#pragma once
#include <string>
#include "SFML/System/Vector2.hpp"
#include "RoadJunctionGeometry.hpp"
#include <vector>



class VehicleSourceSink {

	std::string label;
	bool active = false;
	float period;
	float lastUpdate = 0.f;
	std::string vehicleColour;

	const RoadJunctionGeometry* source;
	const RoadJunctionGeometry* sink;

	int pathId;

	// hardcoded, which is not ideal, but vehicles will get proper colours sometime in the future anyway
	std::vector<std::string> colours = { "b", "g", "o", "r", "w" };

public:

	VehicleSourceSink(const std::string& label, float period, int pathId, const std::string &vehColour, const RoadJunctionGeometry* source, const RoadJunctionGeometry* sink)
		: label(label), period(period), pathId(pathId), vehicleColour(vehColour), source(source), sink(sink) {}

	// time is in seconds
	void update(float time) { lastUpdate = time; }
	void setLabel(const std::string& label) { this->label = label; }
	void setActive(bool active, float time) { this->active = active; this->lastUpdate = time; }
	void setPeriod(float period) { this->period = period; }
	void setVehicleColour(int colour) {
	
		vehicleColour = colours[colour % 5];
	}

	bool isActive() const { return active; }
	int getPathId() const { return pathId; }

	float getPeriod() const { return period; }
	float getLastUpdate() const { return lastUpdate; }

	std::string getLabel() const { return label; }
	std::string getVehicleColour() const { return vehicleColour; }
	int getVehicleColourInt() const {

		for (int i{ 0 }; i < colours.size(); i++)
			if (colours[i] == vehicleColour)
				return i;

		return 0;
	}

	const RoadJunctionGeometry* getSourceJunction() const { return source; }
	const RoadJunctionGeometry* getSinkJunction() const { return sink; }
};


