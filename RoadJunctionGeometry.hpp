


#pragma once
#include <vector>
#include <optional>
#include "RoadSegmentGeometry.hpp"
#include "SFML/System/Vector2.hpp"
#include "RoadGraphTypes.hpp"
#include "Config.hpp"



class RoadJunctionGeometry {

	int id;
	RoadVertexDescriptor vertex;
	std::vector<const RoadSegmentGeometry *> roads;
	std::vector<sf::Vector2f> junctionPoints;

	// RoadNodeData
	sf::Vector2f position;

	

	void sortJunctionRoadsClockwise(std::vector<const RoadSegmentGeometry*> &roads, sf::Vector2f junctionPos);
	void calculateJunctionPoints(const ConfigGlobal &config);
	std::optional<sf::Vector2f> getFurthestClosestIntersection(const RoadSegmentGeometry& road, const RoadSegmentGeometry& roadBefore, const RoadSegmentGeometry& roadAfter, float roadWidth, bool& isBefore) const;

public:

	RoadJunctionGeometry(int id, RoadVertexDescriptor vertex, sf::Vector2f position, std::vector<const RoadSegmentGeometry *> roads, const ConfigGlobal &config);

	// returns vector packed with these four points: side intersection point, side intersection mirror point, road crossing point 1, road crossing point 2
	const std::vector<sf::Vector2f> &getJunctionPoints() const { return junctionPoints; }
	
	sf::Vector2f getPosition() const { return position; }
	RoadVertexDescriptor getVertex() const { return vertex; }
	std::size_t getConnectedCount() const { return roads.size(); }

	int getId() const { return id; }
};


