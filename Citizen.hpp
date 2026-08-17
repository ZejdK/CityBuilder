


#pragma once
#include "RoadGraphTypes.hpp"
#include <string>
#include <vector>
#include "CitizenPosition.hpp"



class Citizen {

	std::string name;
	std::string surname;
	std::string colour;

	int currentVertexId;
	std::vector<RoadVertexDescriptor> path;

	float s; // parameter showing distance progress along the edge

public:
	Citizen(std::string name, std::string surname, std::vector<RoadVertexDescriptor> path, std::string colour);

	bool update(float dt, float edgeDistance);

	CitizenPosition getPositionalData() const;
	std::string getColour() const { return colour; }
};


