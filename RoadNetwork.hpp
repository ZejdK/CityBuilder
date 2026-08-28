


#pragma once
#include <vector>
#include "RoadGraphTypes.hpp"
#include "RoadSegmentGeometry.hpp"
#include <string>
#include <optional>
#include "SFML/System/Vector2.hpp"
#include "RandomCycleFinder.hpp"
#include <utility>



class RoadNetwork {

	RoadGraph roadGraph;

	void addEdge(RoadVertexDescriptor source, RoadVertexDescriptor target);
	RoadVertexDescriptor addVertex(sf::Vector2f pos);

public:
	
	RoadNetwork();

	const RoadGraph& getGraph() const;
	const std::vector<RoadVertexDescriptor> getJunctions() const;

	const std::optional<RoadEdgeDescriptor> findHoveredRoad(float roadWidth, sf::Vector2f cursorPos) const;
	const std::optional<RoadVertexDescriptor> findJunctionNear(float snapRadius, sf::Vector2f position) const;
	std::optional<sf::Vector2f> findJunctionPosNear(float snapRadius, sf::Vector2f position) const;

	std::vector<RoadSegmentGeometry> getJunctionRoadsClockwise(RoadVertexDescriptor junctionVertex) const;

	std::string getInformation() const;
	int getJunctionCount() const;

	std::pair<RoadVertexDescriptor, RoadVertexDescriptor> add(RoadVertexDescriptor source, RoadVertexDescriptor target);
	std::pair<RoadVertexDescriptor, RoadVertexDescriptor> add(sf::Vector2f sourcePos, RoadVertexDescriptor target);
	std::pair<RoadVertexDescriptor, RoadVertexDescriptor> add(RoadVertexDescriptor source, sf::Vector2f targetPos);
	std::pair<RoadVertexDescriptor, RoadVertexDescriptor> add(sf::Vector2f sourcePos, sf::Vector2f targetPos);

	std::vector<RoadVertexDescriptor> getRandomCycle() const {

		return RandomCycleFinder::getRandomCycle(roadGraph);
	}
};


