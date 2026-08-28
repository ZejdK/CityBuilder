


#pragma once
#include "RoadGraphTypes.hpp"
#include <string>
#include <vector>
#include "CitizenPosition.hpp"



class Citizen {

	int id;
	std::string name;
	std::string surname;
	std::string colour;

	int currentVertexId;
	std::vector<RoadVertexDescriptor> path;

	float s; // parameter showing distance progress along the edge

	bool onSameRoad(const Citizen &otherCitizen);
	bool shouldStop(float edgeDistance, const std::vector<Citizen> &citizens, float allowedDistance);

public:
	Citizen(int id, std::string name, std::string surname, std::vector<RoadVertexDescriptor> path, std::string colour);

	bool update(float dt, float edgeDistance, const std::vector<Citizen> &citizens);

	CitizenPosition getPositionalData() const;
	std::string getColour() const { return colour; }
};


