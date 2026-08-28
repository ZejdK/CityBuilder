


#include "Citizen.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include "RoadGraphTypes.hpp"
#include "CitizenPosition.hpp"
#include <cstdlib>



Citizen::Citizen(int id, std::string name, std::string surname, std::vector<RoadVertexDescriptor> path, std::string colour):
	id(id), name(name), surname(surname), path(path), colour(colour), s(0.f) {

	if (path.size() <= 1)
		throw std::invalid_argument("Citizen path cannot be empty or only a single vertex");

	currentVertexId = 0;
}

// NOTE: Citizen should not be aware of other citizens, still prototyping
bool Citizen::update(float dt, float edgeDistance, const std::vector<Citizen> &citizens) {

	const float ALLOWED_DISTANCE { 50.f };
	const float speed { 140.f };
	
	if (!shouldStop(edgeDistance, citizens, ALLOWED_DISTANCE))
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



// NOTE: I can get away with this check bc currently cars run in cycles, which have duplicate info in first and last vertex
// TODO: this will break when vehicles get fixed paths
bool Citizen::onSameRoad(const Citizen &otherCitizen) {

	return path[currentVertexId] == otherCitizen.path[otherCitizen.currentVertexId] &&
		   path[currentVertexId + 1] == otherCitizen.path[otherCitizen.currentVertexId + 1];
}

bool Citizen::shouldStop(float edgeDistance, const std::vector<Citizen> &citizens, float allowedDistance) {

	for (const auto& otherCitizen : citizens) {

		if (&otherCitizen != this && onSameRoad(otherCitizen)) {

			float distance { (otherCitizen.s - s) * edgeDistance };

			if (otherCitizen.s > s && distance < allowedDistance)
				return true;

			if (std::abs(otherCitizen.s - s) < 0.00001f && id > otherCitizen.id)
				return true;
		}
	}

	return false;
}


