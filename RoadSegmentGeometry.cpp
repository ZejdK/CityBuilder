


#include "RoadSegmentGeometry.hpp"
#include <array>
#include <optional>
#include "Math.hpp"
#include "SFML/System/Vector2.hpp"
#include "RoadGraphTypes.hpp"



RoadSegmentGeometry::RoadSegmentGeometry(int id, RoadVertexDescriptor vertexA, RoadVertexDescriptor vertexB, sf::Vector2f start, sf::Vector2f end)
	: id(id), vertexA(vertexA), vertexB(vertexB),  start(start), end(end) {}



// since there's no order which segment point is start or end, endpoint argument helps determine the vertices orientation
std::array<sf::Vector2f, 4> RoadSegmentGeometry::getVertices(float roadWidth, sf::Vector2f endpoint) const {

	if (CB::Math::distance(end, endpoint) < CB::Math::epsilon)
		return getVerticesInternal(end, start, roadWidth);
	else
		return getVerticesInternal(start, end, roadWidth);
}

std::array<sf::Vector2f, 4> RoadSegmentGeometry::getVerticesInternal(sf::Vector2f start, sf::Vector2f end, float roadWidth) const {
	
	sf::Vector2f direction{ (end - start).normalized() };
	sf::Vector2f normal{ -direction.y, direction.x }; // rotation matrix for pi/2
	sf::Vector2f offset{ normal * roadWidth };

	return {
		start + offset, // left start
		end + offset,   // left end
		end - offset,   // right start
		start - offset  // right end
	};
}

std::array<std::optional<sf::Vector2f>, 4> RoadSegmentGeometry::getSideIntersectionPoints(float roadWidth, const RoadSegmentGeometry& other, sf::Vector2f endpoint) const {

	// idea here is to get lines parallel to the road segments, offset for roadWidth in both directions
	// and then find their intersections

	auto vertices = getVertices(roadWidth, endpoint);
	auto otherVertices = other.getVertices(roadWidth, endpoint);

	return {

		CB::Math::intersect(vertices[0], vertices[1], otherVertices[0], otherVertices[1]),
		CB::Math::intersect(vertices[0], vertices[1], otherVertices[3], otherVertices[2]),
		CB::Math::intersect(vertices[3], vertices[2], otherVertices[0], otherVertices[1]),
		CB::Math::intersect(vertices[3], vertices[2], otherVertices[3], otherVertices[2])
	};
}

std::optional<sf::Vector2f> RoadSegmentGeometry::getClosestSideIntersection(float roadWidth, const RoadSegmentGeometry& other, sf::Vector2f roadJunctionPoint) const {

	auto lineIntersections = getSideIntersectionPoints(roadWidth, other, roadJunctionPoint);

	std::optional<sf::Vector2f> closestIntersection = std::nullopt;
	for (const auto& lineIntersection : lineIntersections)
		if (lineIntersection && (!closestIntersection || CB::Math::distance(*lineIntersection, roadJunctionPoint) < CB::Math::distance(*closestIntersection, roadJunctionPoint)))
			closestIntersection = lineIntersection;

	return closestIntersection;
}

std::optional<sf::Vector2f> RoadSegmentGeometry::getFurthestSideIntersection(float roadWidth, const RoadSegmentGeometry& other, sf::Vector2f roadJunctionPoint) const {

	auto lineIntersections = getSideIntersectionPoints(roadWidth, other, roadJunctionPoint);

	std::optional<sf::Vector2f> closestIntersection = std::nullopt;
	for (const auto& lineIntersection : lineIntersections)
		if (lineIntersection && (!closestIntersection || CB::Math::distance(*lineIntersection, roadJunctionPoint) > CB::Math::distance(*closestIntersection, roadJunctionPoint)))
			closestIntersection = lineIntersection;

	return closestIntersection;
}

sf::Vector2f RoadSegmentGeometry::getPointOnShoulder(float roadWidth, float distance, bool leftSide, sf::Vector2f endpoint) const {

	auto vertices = getVertices(roadWidth, endpoint);

	int i1 = 0, i2 = 1; // right side
	if (!leftSide)
		i1 = 3, i2 = 2; // left side

	return vertices[i1] + (vertices[i2] - vertices[i1]).normalized() * distance;
}

sf::Vector2f RoadSegmentGeometry::getProjectedPoint(const sf::Vector2f& point) const {

	sf::Vector2f lineVec = end - start;
	float lineLengthSq = lineVec.x * lineVec.x + lineVec.y * lineVec.y;

	sf::Vector2f pointVec = point - start;

	float dotProduct = pointVec.x * lineVec.x + pointVec.y * lineVec.y;
	float t = dotProduct / lineLengthSq;

	return start + t * lineVec;
}

sf::Vector2f RoadSegmentGeometry::getMirroredPoint(sf::Vector2f point) const {

	sf::Vector2f segment = end - start;
	sf::Vector2f toPoint = point - start;

	float dotProduct = segment.x * toPoint.x + segment.y * toPoint.y;
	float segmentLengthSq = segment.x * segment.x + segment.y * segment.y;

	if (segmentLengthSq < CB::Math::epsilon)
		return start * 2.0f - point; // Mirrors point across a single spot

	sf::Vector2f closestPoint = start + (dotProduct / segmentLengthSq) * segment;
	return 2.0f * closestPoint - point;
}

bool RoadSegmentGeometry::isPointOnRoad(float roadWidth, const sf::Vector2f& point) const {

	auto vertices = getVertices(roadWidth, getStart());

	sf::Vector2f e0 = vertices[1] - vertices[0];
	sf::Vector2f e1 = vertices[2] - vertices[1];
	sf::Vector2f e2 = vertices[3] - vertices[2];
	sf::Vector2f e3 = vertices[0] - vertices[3];

	sf::Vector2f p0 = point - vertices[0];
	sf::Vector2f p1 = point - vertices[1];
	sf::Vector2f p2 = point - vertices[2];
	sf::Vector2f p3 = point - vertices[3];

	bool c0 = e0.cross(p0) >= 0.0f;
	bool c1 = e1.cross(p1) >= 0.0f;
	bool c2 = e2.cross(p2) >= 0.0f;
	bool c3 = e3.cross(p3) >= 0.0f;

	return (c0 == c1) && (c1 == c2) && (c2 == c3);
}


