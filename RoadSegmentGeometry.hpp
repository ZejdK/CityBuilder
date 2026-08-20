


#pragma once
#include <SFML/System/Vector2.hpp>
#include <array>
#include <optional>



class RoadSegmentGeometry {

public: // TODO: members should be private
	sf::Vector2f start;
	sf::Vector2f end;
// public:

	RoadSegmentGeometry(sf::Vector2f start, sf::Vector2f end);

	std::array<sf::Vector2f, 4> getVertices(float roadWidth) const;

	std::array<std::optional<sf::Vector2f>, 4> getSideIntersectionPoints(float roadWidth, const RoadSegmentGeometry& other) const;
	std::optional<sf::Vector2f> getClosestSideIntersection(float roadWidth, const RoadSegmentGeometry& other, sf::Vector2f roadJunctionPoint) const;
	std::optional<sf::Vector2f> getFurthestSideIntersection(float roadWidth, const RoadSegmentGeometry& other, sf::Vector2f roadJunctionPoint) const;

	sf::Vector2f getPointAlongSide(float roadWidth, float distance, bool leftSide) const;
	sf::Vector2f getProjectedPoint(const sf::Vector2f& point) const;
	sf::Vector2f getMirroredPoint(sf::Vector2f point) const;

	bool isPointOnRoad(float roadWidth, const sf::Vector2f& point) const;

	float length() const;
};


