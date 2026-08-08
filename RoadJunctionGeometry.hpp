


#pragma once
#include <vector>
#include <optional>
#include "RoadSegmentGeometry.hpp"



class RoadJunctionGeometry {

	std::vector<RoadSegmentGeometry> roads;
	sf::Vector2f position;

	std::optional<sf::Vector2f> getFurthestClosestIntersection(const RoadSegmentGeometry& road, const RoadSegmentGeometry& roadBefore, const RoadSegmentGeometry& roadAfter, float roadWidth, bool& isBefore) const;

public:

	RoadJunctionGeometry(sf::Vector2f position, const std::vector<RoadSegmentGeometry>& roads);

	// returns vector packed with these four points: side intersection point, side intersection mirror point, road crossing point 1, road crossing point 2
	std::vector<sf::Vector2f> getJunctionPoints(float roadWidth, float roadCrossingSegmentLength) const;
	sf::Vector2f getPosition() const;
};


