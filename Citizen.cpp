


#include "Citizen.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include "RoadGraphTypes.hpp"
#include "CitizenPosition.hpp"



Citizen::Citizen(std::string name, std::string surname, std::vector<RoadVertexDescriptor> path, std::string colour):
	name(name), surname(surname), path(path), colour(colour), s(0.f) {

	if (path.size() <= 1)
		throw std::invalid_argument("Citizen path cannot be empty or only a single vertex");

	currentVertexId = 0;
}

bool Citizen::update(float dt, float edgeDistance) {

	const float speed { 140.f };

	// s is parameter showing distance progress along the edge
	s += (speed * dt) / edgeDistance;

	if (s >= 1.f) {

		++currentVertexId;
		s = 0.f;

		if (currentVertexId + 1 == path.size())
			currentVertexId = 0;
	}

	return false;
}

CitizenPosition Citizen::getPositionalData() const {

	return { path[currentVertexId], path[currentVertexId + 1], s };
}


