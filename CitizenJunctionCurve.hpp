


#pragma once
#include <SFML/System/Vector2.hpp>
#include "CitizenSimulation.hpp"
#include "RoadJunctionGeometry.hpp"
#include "RoadSegmentGeometry.hpp"
#include <tuple>
#include <utility>




static sf::Vector2f laneOffset(sf::Vector2f fromPos, sf::Vector2f toPos) {

	float offset{ 20.f }; /// SHOULD BE ROAD_WIDTH / 2

	auto dir = (toPos - fromPos).normalized();
	auto normal = sf::Vector2f(-dir.y, dir.x);

	return normal * offset;
}

struct CitizenJunctionCurve {

	sf::Vector2f entryPos;
	sf::Vector2f exitPos;

	sf::Vector2f entryDir;
	sf::Vector2f exitDir;

	sf::Vector2f entryControlPos;
	sf::Vector2f exitControlPos;

	float sJunIn;
	float sJunOut;



	// NOTE: won't be called if the citizen is on the first or final edge
	CitizenJunctionCurve(float s, const CitizenSimulation::CitizenLayoutContext& layoutContext) {

		// when the citizen is inside a junction, this class is created every frame to calculate the position and direction of the citizen along the junction curve
		// reason why these need to order is because in the midpoint, citizen will advance its current edge and therefore its layout context
		auto [ road, junction, nextRoad ] { ensureLayoutOrder(s, layoutContext) };

		auto junctionPos = junction->getPosition();
		auto startPos{ road->getOtherEndpoint(junctionPos) };
		auto endPos{ nextRoad->getOtherEndpoint(junctionPos) };

		entryPos = junction->getSPos(road) + laneOffset(startPos, junctionPos);
		exitPos = junction->getSPos(nextRoad) + laneOffset(junctionPos, endPos);
		sJunIn = junction->getS(road);
		sJunOut = junction->getS(nextRoad);

		entryDir = (junctionPos - startPos).normalized();
		exitDir = (endPos - junctionPos).normalized();

		float entryControlDistance = (junctionPos - junction->getSPos(road)).length() * 0.75f;
		float exitControlDistance = (junctionPos - junction->getSPos(nextRoad)).length() * 0.75f;

		entryControlPos = entryPos + entryDir * entryControlDistance;
		exitControlPos = exitPos - exitDir * exitControlDistance;
	}

	// makes sure they're in order startPos - junction - endPos
	std::tuple<const RoadSegmentGeometry*, const RoadJunctionGeometry*, const RoadSegmentGeometry*> ensureLayoutOrder(float s, const CitizenSimulation::CitizenLayoutContext& layoutContext) const {

		if (isBeforeJunctionCenter(s))
			return { layoutContext.previousRoad, layoutContext.outgoingJunction, layoutContext.road     }; // citizen is on the edge after the junction - s in [0, sJunOut]
		else
			return { layoutContext.road,         layoutContext.incomingJunction, layoutContext.nextRoad }; // citizen on the edge before the junction - s in [1-sJunIn, 1]
	}

	std::pair<sf::Vector2f, sf::Vector2f> getCitizenPosAndDir(float s) const {

		float t{ mapStoT(s, sJunIn, sJunOut) };

		auto citizenPos = bezier(t, entryPos, entryControlPos, exitControlPos, exitPos);
		auto citizenDir = bezierDerivative(t, entryPos, entryControlPos, exitControlPos, exitPos);

		return { citizenPos, citizenDir };
	}



	// NOTE: comparing s with 0.5 works, but it's a hack
	// UI needs to make sure values close to 0.5 don't happen
	bool isBeforeJunctionCenter(float s) const {

		return s < 0.5f;
	}
	
	// s needs to be in [0, sJunOut] U [1-sJunIn, 1]
	// t needs to be in [0, 1]
	float mapStoT(float s, float sJunIn, float sJunOut) const {
		
		return isBeforeJunctionCenter(s)
			? 0.5f + 0.5f * s / sJunOut				// map [0, sJunOut]  to [0.5, 1]
			: 0.5f / sJunIn * (s - (1.f - sJunIn)); // map [1-sJunIn, 1] to [0, 0.5]
	}

	// p0 entry point, p1 entry control point, p2 exit control point, p3 exit point
	sf::Vector2f bezier(float t, sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3) const {

		auto k{ 1 - t };

		return  k * k * k * p0 +
			3 * k * k * t * p1 +
			3 * k * t * t * p2 +
				t * t * t * p3;
	}

	sf::Vector2f bezierDerivative(float t, sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3) const {
	
		auto k{ 1 - t };
	
		return 3 * k * k * (p1 - p0) +
			   6 * k * t * (p2 - p1) +
			   3 * t * t * (p3 - p2);
	}
};


