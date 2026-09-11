


#include "CitizenSimulation.hpp"
#include <string>
#include <vector>
#include "Citizen.hpp"
#include "RoadNetwork.hpp"
#include <array>
#include "VehiclePath.hpp"
#include <optional>
#include <cstdlib>
#include "RoadJunctionGeometry.hpp"
#include "RoadSegmentGeometry.hpp"
#include <tuple>
#include <utility>
#include "SFML/System/Vector2.hpp"



void CitizenSimulation::enableTest() {

	std::array<std::string, 5> colours { "b", "g", "w", "r", "o" };

	for (int i { 0 }; i < 50; ++i) {

		int id{ int(citizens.size()) };

		citizens.push_back(Citizen{ id, i % 2 == 0 ? "John" : "Jane", "Doe", colours[i % 5]});
		Citizen& citizen{ citizens.back() };

		vehiclePaths.push_back(VehiclePath{ id, roadNetwork.getRandomCycle() });
		VehiclePath &vehPath{ vehiclePaths.back() };
		
		citizen.activate(vehPath.getId());
	}

	enabled = true;
}

void CitizenSimulation::update(float dt) {
	
	if (roadNetwork.getLayout().getInformation().junctionCount < 5)
		return;

	if (!enabled)
		enableTest();
	else
		realUpdate(dt);
}



void CitizenSimulation::realUpdate(float dt) {

	for (auto& citizen : citizens) {

		// get
		auto layoutContext{ getCitizenLayoutContext(citizen) };
		auto vehPath{ getVehiclePath(citizen.getPathId()) };

		// calculate values
		auto [ newS, advanceEdge, insideJunction ] { getNewValues(citizen, layoutContext, vehPath, dt) };

		// update
		if (advanceEdge)
			vehPath->advanceEdge();
		citizen.update(newS, insideJunction);
	}
}



CitizenSimulation::CitizenLayoutContext CitizenSimulation::getCitizenLayoutContext(const Citizen& citizen) const {

	auto vehPath{ getVehiclePath(citizen.getPathId()) };

	auto previousEdge{ vehPath->getPreviousEdge() };
	auto currentEdge{ vehPath->getCurrentEdge() };
	auto nextEdge{ vehPath->getNextEdge() };

	auto [ tail, head ] { roadNetwork.getEdgeVertices(*currentEdge) };
	auto &roadLayout{ roadNetwork.getLayout() };

	return {
		previousEdge ? roadLayout.getRoad(*previousEdge) : nullptr,
		currentEdge ? roadLayout.getRoad(*currentEdge) : nullptr,
		nextEdge ? roadLayout.getRoad(*nextEdge) : nullptr,
		roadLayout.getJunction(head),
		roadLayout.getJunction(tail),
		roadLayout.getJunction(head)->getConnectedRoads()
	};
}



std::pair<bool, bool> CitizenSimulation::getMovementChecks(const Citizen& citizen, const CitizenLayoutContext &layoutContext) const {

	bool insideJunction = isInsideJunction(citizen, layoutContext);
	bool carAhead{ false };
	for (const auto& otherCitizen : citizens) {

		if (!citizen.isActive() || !otherCitizen.isActive() || otherCitizen.getId() == citizen.getId())
			continue;

		carAhead = isTooCloseAheadOnTheSameEdge(citizen, otherCitizen, layoutContext.road->length());

		if (carAhead)
			break;
	}

	return { carAhead, insideJunction };
}

std::tuple<float, bool, bool> CitizenSimulation::getNewValues(Citizen& citizen, const CitizenLayoutContext& layoutContext, VehiclePath *vehPath, float dt) const {

	auto [ carAhead, insideJunction ]{ getMovementChecks(citizen, layoutContext) };
	
	const float speed{ 140.f };
	float newS{ citizen.getS() };
	bool advanceEdge{ false };

	bool shouldWait{ carAhead };
	if (!shouldWait) {

		newS += (speed * dt) / layoutContext.road->length();
		if (newS >= 1.f) {

			advanceEdge = true;
			newS = 0.f;
		}
	}

	return { newS, advanceEdge, insideJunction };
}



// NOTE: I can get away with this check bc currently cars run in cycles, which have duplicate info in first and last vertex
// TODO: this will break when vehicles get fixed paths
// 
// all of the checks assume that both citizen are active (and therefore have state) and that they are different
bool CitizenSimulation::onSameEdge(const Citizen& citizen, const Citizen& otherCitizen) const {

	const VehiclePath* vehPath = getVehiclePath(citizen.getPathId());
	const VehiclePath* otherVehPath = getVehiclePath(otherCitizen.getPathId());

	return vehPath->getCurrentEdge() == otherVehPath->getCurrentEdge();
}

bool CitizenSimulation::isTooCloseAheadOnTheSameEdge(const Citizen& citizen, const Citizen& otherCitizen, float edgeDistance) const {

	if (!onSameEdge(citizen, otherCitizen))
		return false;

	float distance{ (otherCitizen.getS() - citizen.getS()) * edgeDistance};

	if (otherCitizen.getS() > citizen.getS() && distance < ALLOWED_DISTANCE)
		return true;

	if (std::abs(otherCitizen.getS() - citizen.getS()) < 0.00001f && citizen.getId() > otherCitizen.getId())
		return true;

	return false;
}

bool CitizenSimulation::isInsideJunction(const Citizen &citizen, const CitizenLayoutContext &layoutContext) const {

	return citizen.getS() <= layoutContext.incomingJunction->getS(layoutContext.road) ||
		   citizen.getS() >= 1.f - layoutContext.incomingJunction->getS(layoutContext.nextRoad);
}

std::pair<sf::Vector2f, sf::Vector2f> CitizenSimulation::getCitizenDirection(int citizenId) const {

	auto edge{ getVehiclePath(citizenId)->getCurrentEdge() };
	auto [tail, head] { roadNetwork.getEdgeVertices(*edge) };

	auto fromPos{ roadNetwork.getGraph()[tail].position };
	auto toPos{ roadNetwork.getGraph()[head].position };

	return { fromPos, toPos };
}


