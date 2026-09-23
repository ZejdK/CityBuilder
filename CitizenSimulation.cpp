


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
#include "CitizenLayoutContext.hpp"
#include <utility>
#include "SFML/System/Vector2.hpp"
#include <cmath>
#include <numbers>
#include "CitizenJunctionCurve.hpp"
#include <tuple>
#include "Math.hpp"



void CitizenSimulation::update(float totalTime, float dt) {

	this->totalTime = totalTime;

	updateCitizens(totalTime, dt);
	updateVehicleSourceSinks(totalTime, dt);
}

void CitizenSimulation::updateCitizens(float totalTime, float dt) {

	activeCitizenCount = 0;

	for (auto& citizen : citizens) {

		if (!citizen.isActive())
			continue;

		++activeCitizenCount;

		// get
		auto layoutContext{ getCitizenLayoutContext(citizen) };
		auto vehPath{ getVehiclePath(citizen.getPathId()) };

		// calculate values
		auto [ newState, advanceEdge ] { getNewValues(citizen, layoutContext, vehPath, totalTime, dt) };

		// update
		if (advanceEdge)
			vehPath->advanceEdge();

		if (vehPath->completed())
			citizen.disable();
		else
			citizen.update(newState);
	}
}

void CitizenSimulation::updateVehicleSourceSinks(float totalTime, float dt) {
	
	for (auto& vehSourceSink : vehicleSourceSinks) {

		if (vehSourceSink.isActive() && totalTime - vehSourceSink.getLastUpdate() > vehSourceSink.getPeriod()) {

			auto newPath{ copyVehiclePath(vehSourceSink.getPathId()) };

			createNewCitizen("Place", "Holder", vehSourceSink.getVehicleColour())
				.activate(newPath->getId());

			vehSourceSink.update(totalTime);
		}
	}
}



Citizen &CitizenSimulation::createNewCitizen(const std::string& name, const std::string& surname, const std::string& colour) {

	int id{ int(citizens.size()) };
	citizens.push_back(Citizen{ id, name, surname, colour });

	return citizens.back();
}

const Citizen* CitizenSimulation::findCitizen(sf::Vector2f position) const {
	
	for (const auto& citizen : citizens)
		if (citizen.isActive() && CB::Math::distance(citizen.getPosition(), position) < config.vehicleSnapRadius)
			return &citizen;

	return nullptr;
}



CitizenLayoutContext CitizenSimulation::getCitizenLayoutContext(const Citizen& citizen) const {

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
		roadLayout.getJunction(tail)
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

std::pair<CitizenState, bool> CitizenSimulation::getNewValues(Citizen& citizen, const CitizenLayoutContext& layoutContext, VehiclePath *vehPath, float totalTime, float dt) const {

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

	auto [ position, direction, curve ] { getNewPositionAndDirection(citizen) };
	auto [ indRight, indLeft ] { getIndicators(citizen, newS, totalTime, layoutContext) };

	CitizenState newState{ citizen.getPathId(), insideJunction, newS, indRight, indLeft, position, direction, curve };
	return { newState, advanceEdge };
}

std::pair<bool, bool> CitizenSimulation::getIndicators(const Citizen& citizen, float newS, float totalTime, const CitizenLayoutContext& layoutContext) const {

	const float INDICATOR_DISTANCE{ 200.f };

	if (isCitizenOnLastEdge(citizen)
		|| layoutContext.incomingJunction->getConnectedCount() < 3
		|| (1 - newS) * layoutContext.road->length() > INDICATOR_DISTANCE)
		return { false, false };

	
	float phaseRandomness { citizen.getId() % 7 * 0.47f };
	bool blinkOn{ std::sin(4 * std::numbers::pi * totalTime + phaseRandomness) < 0 };

	RoadJunctionGeometry::NextTurn nextTurn{ layoutContext.incomingJunction->getNextTurnDirection(layoutContext.road->getId(), layoutContext.nextRoad->getId()) };

	switch (nextTurn) {
	case RoadJunctionGeometry::NextTurn::Left:
		return { false, blinkOn };
		break;

	case RoadJunctionGeometry::NextTurn::Right:
		return { blinkOn, false };
		break;
	}

	return { false, false };
}

std::tuple<sf::Vector2f, sf::Vector2f, std::optional<CitizenJunctionCurve>> CitizenSimulation::getNewPositionAndDirection(const Citizen& citizen) const {

	auto layoutContext{ getCitizenLayoutContext(citizen) };

	if (!isCitizenStartingPath(citizen) && citizen.isInsideJunction()) {

		CitizenJunctionCurve curve{ citizen.getS(), layoutContext, config.roadWidth };
		auto [ pos, dir ] { curve.getCitizenPosAndDir(citizen.getS()) };

		return { pos, dir, curve };
	}
	else {

		auto [ fromPos, toPos ] { getCitizenDirection(citizen) };
		auto offset = RoadSegmentGeometry::getLaneOffset(fromPos, toPos, config.roadWidth);

		auto pos = CB::Math::lerp(fromPos + offset, toPos + offset, citizen.getS());
		auto dir = toPos - fromPos;

		return { pos, dir, std::nullopt };
	}
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
	
	bool nextJunctionExists{ layoutContext.incomingJunction->getConnectedCount() > 1 };
	bool nextRoadExists{ layoutContext.nextRoad != nullptr };

	bool insideNextJunction{ nextJunctionExists && citizen.getS() <= layoutContext.incomingJunction->getS(layoutContext.road) };
	bool insidePrevJunction{ nextRoadExists && citizen.getS() >= 1.f - layoutContext.incomingJunction->getS(layoutContext.nextRoad) };
	
	return insideNextJunction || insidePrevJunction;
}

std::pair<sf::Vector2f, sf::Vector2f> CitizenSimulation::getCitizenDirection(const Citizen& citizen) const {

	auto edge{ getVehiclePath(citizen.getPathId())->getCurrentEdge() };
	auto [tail, head] { roadNetwork.getEdgeVertices(*edge) };

	auto fromPos{ roadNetwork.getGraph()[tail].position };
	auto toPos{ roadNetwork.getGraph()[head].position };

	return { fromPos, toPos };
}

std::pair<sf::Vector2f, sf::Vector2f> CitizenSimulation::getCitizenPathDirection(const Citizen& citizen, bool next) const {

	auto edge{ getVehiclePath(citizen.getPathId())->getCurrentEdge() };
	if (next)
		edge = getVehiclePath(citizen.getPathId())->getNextEdge();

	auto [ tail, head ] { roadNetwork.getEdgeVertices(*edge) };

	auto fromPos{ roadNetwork.getGraph()[tail].position };
	auto toPos{ roadNetwork.getGraph()[head].position };

	return { fromPos, toPos };
}



// VehiclePath
bool CitizenSimulation::isCitizenOnFirstOrLastEdge(const Citizen& citizen) const
{
	auto vehPath{ getVehiclePath(citizen.getPathId()) };

	return vehPath->getPreviousEdge() == std::nullopt || vehPath->getNextEdge() == std::nullopt;
}

bool CitizenSimulation::isCitizenOnLastEdge(const Citizen& citizen) const
{
	auto vehPath{ getVehiclePath(citizen.getPathId()) };

	return vehPath->getNextEdge() == std::nullopt;
}

bool CitizenSimulation::isCitizenStartingPath(const Citizen& citizen) const
{
	auto vehPath{ getVehiclePath(citizen.getPathId()) };

	return vehPath->getPreviousEdge() == std::nullopt && citizen.getS() < 0.5f;
}

bool CitizenSimulation::isCitizenFinishingPath(const Citizen& citizen) const
{
	auto vehPath{ getVehiclePath(citizen.getPathId()) };

	return vehPath->getNextEdge() == std::nullopt && citizen.getS() > 0.5f;
}


