


#include "RoadJunctionGeometry.hpp"
#include "Math.hpp"
#include <optional>
#include <vector>
#include "SFML/System/Vector2.hpp"
#include "RoadSegmentGeometry.hpp"



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

RoadJunctionGeometry::RoadJunctionGeometry(sf::Vector2f position, const std::vector<RoadSegmentGeometry>& roads)
	: position(position), roads(roads) {}

// returns vector packed with these four points: side intersection point, side intersection mirror point, road crossing point 1, road crossing point 2
std::vector<sf::Vector2f> RoadJunctionGeometry::getJunctionPoints(float roadWidth, float roadCrossingSegmentLength) const {

	std::vector<sf::Vector2f> junctionPoints;

	for (int i{ 0 }; i < roads.size(); ++i) {

		const auto& road = roads[i];
		const auto& roadBefore = roads[(i - 1 + roads.size()) % roads.size()];
		const auto& roadAfter = roads[(i + 1) % roads.size()];
		bool isBefore = false;

		std::optional<sf::Vector2f> closestLineIntersection = getFurthestClosestIntersection(road, roadBefore, roadAfter, roadWidth, isBefore);
		if (closestLineIntersection) {

			auto mirroredPoint = road.getMirroredPoint(*closestLineIntersection);

			float offset = (position - ((*closestLineIntersection + mirroredPoint) / 2.f)).length();

			auto crossingPoint1 = road.getPointAlongSide(roadWidth, roadCrossingSegmentLength / 2 + offset, false);
			auto crossingPoint2 = road.getPointAlongSide(roadWidth, roadCrossingSegmentLength / 2 + offset, true);

			junctionPoints.push_back(isBefore ? mirroredPoint : *closestLineIntersection);
			junctionPoints.push_back(isBefore ? *closestLineIntersection : mirroredPoint);
			junctionPoints.push_back(crossingPoint2);
			junctionPoints.push_back(crossingPoint1);
		}
	}

	return junctionPoints;
}

sf::Vector2f RoadJunctionGeometry::getPosition() const {

	return position;
}


