


#pragma once
#include <SFML/System/Vector2.hpp>
#include <array>
#include "Math.hpp"



struct RoadElement {

	sf::Vector2f start;
	sf::Vector2f end;

	std::array<sf::Vector2f, 4> getVertices(float roadWidth) const {

		sf::Vector2f direction { (end - start).normalized() };
		sf::Vector2f normal { -direction.y, direction.x }; // rotation matrix for pi/2
		sf::Vector2f offset { normal * roadWidth };

		return {

			start + offset, // left start
			end + offset,   // left end
			end - offset,   // right start
			start - offset  // right end
		};
	}

	std::array<std::optional<sf::Vector2f>, 4> getSideIntersectionPoints(float roadWidth, const RoadElement& other) const {

		// idea here is to get lines parallel to the road segments, offset for roadWidth in both directions
		// and then find their intersections

		auto vertices = getVertices(roadWidth);
		auto otherVertices = other.getVertices(roadWidth);

		return {

			CB::Math::intersect(vertices[0], vertices[1], otherVertices[0], otherVertices[1]),
			CB::Math::intersect(vertices[0], vertices[1], otherVertices[3], otherVertices[2]),
			CB::Math::intersect(vertices[3], vertices[2], otherVertices[0], otherVertices[1]),
			CB::Math::intersect(vertices[3], vertices[2], otherVertices[3], otherVertices[2])
		};
	}

	std::optional<sf::Vector2f> getClosestSideIntersection(float roadWidth, const RoadElement& other, sf::Vector2f roadJunctionPoint) const {

		auto lineIntersections = getSideIntersectionPoints(roadWidth, other);
		
		std::optional<sf::Vector2f> closestIntersection = std::nullopt;
		for (const auto& lineIntersection : lineIntersections)
			if (lineIntersection && (!closestIntersection || CB::Math::distance(*lineIntersection, roadJunctionPoint) < CB::Math::distance(*closestIntersection, roadJunctionPoint)))
				closestIntersection = lineIntersection;

		return closestIntersection;
	}

	std::optional<sf::Vector2f> getFurthestSideIntersection(float roadWidth, const RoadElement& other, sf::Vector2f roadJunctionPoint) const {

		auto lineIntersections = getSideIntersectionPoints(roadWidth, other);

		std::optional<sf::Vector2f> closestIntersection = std::nullopt;
		for (const auto& lineIntersection : lineIntersections)
			if (lineIntersection && (!closestIntersection || CB::Math::distance(*lineIntersection, roadJunctionPoint) > CB::Math::distance(*closestIntersection, roadJunctionPoint)))
				closestIntersection = lineIntersection;

		return closestIntersection;
	}

	sf::Vector2f getPointAlongSide(float roadWidth, float distance, bool leftSide) const {

		auto vertices = getVertices(roadWidth);
		
		int i1 = 0, i2 = 1; // right side
		if (!leftSide)
			i1 = 3, i2 = 2; // left side

		return vertices[i1] + (vertices[i2] - vertices[i1]).normalized() * distance;
	}
	
	sf::Vector2f getProjectedPoint(const sf::Vector2f& point) const {
		
		sf::Vector2f lineVec = end - start;
		float lineLengthSq = lineVec.x * lineVec.x + lineVec.y * lineVec.y;

		sf::Vector2f pointVec = point - start;

		float dotProduct = pointVec.x * lineVec.x + pointVec.y * lineVec.y;
		float t = dotProduct / lineLengthSq;

		return start + t * lineVec;
	}

	sf::Vector2f getMirroredPoint(sf::Vector2f point) const {

		sf::Vector2f segment = end - start;
		sf::Vector2f toPoint = point - start;

		float dotProduct = segment.x * toPoint.x + segment.y * toPoint.y;
		float segmentLengthSq = segment.x * segment.x + segment.y * segment.y;

		if (segmentLengthSq < 1e-6f)
			return start * 2.0f - point; // Mirrors point across a single spot

		sf::Vector2f closestPoint = start + (dotProduct / segmentLengthSq) * segment;
		return 2.0f * closestPoint - point;
	}

	float length() const {

		return std::sqrt((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));
	}
};

struct RoadJunctionGeometry {
private:

	std::optional<sf::Vector2f> getFurthestClosestIntersection(const RoadElement& road, const RoadElement& roadBefore, const RoadElement& roadAfter, float roadWidth, bool &isBefore) const {

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
public:
	std::vector<RoadElement> roads;
	sf::Vector2f position;

	// returns vector packed with these four points: side intersection point, side intersection mirror point, road crossing point 1, road crossing point 2
	std::vector<sf::Vector2f> getJunctionPoints(float roadWidth, float roadCrossingSegmentLength) const {

		std::vector<sf::Vector2f> junctionPoints;

		for (int i { 0 }; i < roads.size(); ++i) {

			const auto &road = roads[i];
			const auto &roadBefore = roads[(i - 1 + roads.size()) % roads.size()];
			const auto &roadAfter = roads[(i + 1) % roads.size()];
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

	sf::Vector2f getPosition() const {

		return position;
	}
};


