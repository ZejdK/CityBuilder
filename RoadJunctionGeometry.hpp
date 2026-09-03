


#pragma once
#include <vector>
#include <optional>
#include "RoadSegmentGeometry.hpp"
#include "SFML/System/Vector2.hpp"
#include "RoadGraphTypes.hpp"
#include "Config.hpp"
#include <unordered_map>



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

	std::unordered_map<int, float> roadJunctionSValues; // indexed with RoadSegmentGeometry::getId()

public:

	RoadJunctionGeometry(int id, RoadVertexDescriptor vertex, sf::Vector2f position, std::vector<const RoadSegmentGeometry *> roads, const ConfigGlobal &config);

	// returns vector packed with these four points: side intersection point, side intersection mirror point, road crossing point 1, road crossing point 2
	const std::vector<sf::Vector2f> &getJunctionPoints() const { return junctionPoints; }
	const std::vector<const RoadSegmentGeometry*>& getConnectedRoads() const { return roads; }
	
	sf::Vector2f getPosition() const { return position; }
	RoadVertexDescriptor getVertex() const { return vertex; }
	std::size_t getConnectedCount() const { return roads.size(); }
	
	// returns the value of 's' parameter on the road where the junction begins
	float getS(const RoadSegmentGeometry *road) const { return roadJunctionSValues.at(road->getId()); }
	sf::Vector2f getSPos(const RoadSegmentGeometry* road) const { return getPosition() + road->length() * getS(road) * road->getDirectionVector(getPosition()); }

	int getId() const { return id; }
};


