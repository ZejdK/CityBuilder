


#include "RoadJunctionGeometry.hpp"
#include "Math.hpp"
#include <optional>
#include <vector>
#include "SFML/System/Vector2.hpp"
#include "RoadSegmentGeometry.hpp"
#include "RoadGraphTypes.hpp"
#include "Config.hpp"
#include <cmath>
#include <algorithm>



std::optional<sf::Vector2f> RoadJunctionGeometry::getFurthestClosestIntersection(const RoadSegmentGeometry& road, const RoadSegmentGeometry& roadBefore, const RoadSegmentGeometry& roadAfter, float roadWidth, bool& isBefore) const {

	auto closestLineIntersectionBefore = road.getClosestSideIntersection(roadWidth, roadBefore, position);
	auto closestLineIntersectionAfter = road.getClosestSideIntersection(roadWidth, roadAfter, position);

	if (closestLineIntersectionBefore && closestLineIntersectionAfter) {

		float dist1 = CB::Math::distance(*closestLineIntersectionBefore, position);
		float dist2 = CB::Math::distance(*closestLineIntersectionAfter, position);

		if (dist1 > dist2) {
			isBefore = true;
			return closestLineIntersectionBefore;
		}
		else {
			isBefore = false;
			return closestLineIntersectionAfter;
		}
	}
	else if (closestLineIntersectionBefore)
		return closestLineIntersectionBefore;
	else if (closestLineIntersectionAfter)
		return closestLineIntersectionAfter;

	return std::nullopt;
}

RoadJunctionGeometry::RoadJunctionGeometry(int id, RoadVertexDescriptor vertex, sf::Vector2f position, const std::vector<const RoadSegmentGeometry *> junctionRoads, const ConfigGlobal &config)
	: id(id), vertex(vertex), position(position), roads(junctionRoads) {

	sortJunctionRoadsClockwise(roads, position);
	calculateJunctionPoints(config);
}

// calculates its road shoulders intersection points and road crossing points
// also calculates what value of 's' parameter on the road the junction begins
void RoadJunctionGeometry::calculateJunctionPoints(const ConfigGlobal &config) {

	junctionPoints.clear();
	roadJunctionSValues.clear();

	for (int i{ 0 }; i < roads.size() ; ++i) {

		const auto road = roads[i];
		const auto roadBefore = roads[(i - 1 + roads.size()) % roads.size()];
		const auto roadAfter = roads[(i + 1) % roads.size()];
		bool isBefore = false;

		std::optional<sf::Vector2f> closestLineIntersection = getFurthestClosestIntersection(*road, *roadBefore, *roadAfter, config.roadWidth, isBefore);
		if (closestLineIntersection) {

			auto mirroredPoint = road->getMirroredPoint(*closestLineIntersection);

			float offset = (position - ((*closestLineIntersection + mirroredPoint) / 2.f)).length();

			auto crossingPoint1 = road->getPointOnShoulder(config.roadWidth, config.roadCrossingSegmentLength / 2 + offset, true, position);
			auto crossingPoint2 = road->getPointOnShoulder(config.roadWidth, config.roadCrossingSegmentLength / 2 + offset, false, position);

			junctionPoints.push_back(isBefore ? mirroredPoint : *closestLineIntersection);
			junctionPoints.push_back(isBefore ? *closestLineIntersection : mirroredPoint);
			junctionPoints.push_back(crossingPoint2);
			junctionPoints.push_back(crossingPoint1);

			sf::Vector2f midpoint = (mirroredPoint + *closestLineIntersection + crossingPoint1 + crossingPoint2) * 0.25f;
			roadJunctionSValues.insert({ roads[i]->getId(), (midpoint - position).length() / road->length() });
		}
	}
}

void RoadJunctionGeometry::sortJunctionRoadsClockwise(std::vector<const RoadSegmentGeometry *> &roads, sf::Vector2f junctionPos) {

	auto getAngle = [&](const RoadSegmentGeometry *road) {

		sf::Vector2f A{ road->getStart() }, B{ road->getEnd() };
		sf::Vector2f otherPos{ A };

		if (CB::Math::distance(A, junctionPos) < CB::Math::distance(B, junctionPos))
			otherPos = B;
		
		return std::atan2(otherPos.y - junctionPos.y, otherPos.x - junctionPos.x);
	};

	std::sort(roads.begin(), roads.end(), [&](const RoadSegmentGeometry *a, const RoadSegmentGeometry *b) {

		return getAngle(a) < getAngle(b);
	});
}


