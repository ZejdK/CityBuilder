


#pragma once
#include <vector>
#include "RoadGraphTypes.hpp"
#include "RoadSegmentGeometry.hpp"
#include <string>



class RoadNetwork {

	RoadGraph roadGraph;

	void addEdge(RoadVertexDescriptor source, RoadVertexDescriptor target);
	RoadVertexDescriptor addVertex(sf::Vector2f pos);

public:
	
	RoadNetwork();

	const RoadGraph& getGraph() const;
	const std::vector<RoadVertexDescriptor> getJunctions() const;

	const std::optional<RoadVertexDescriptor> findJunctionNear(float snapRadius, sf::Vector2f position) const;
	std::optional<sf::Vector2f> findJunctionPosNear(float snapRadius, sf::Vector2f position) const;

	std::vector<RoadSegmentGeometry> getJunctionRoadsClockwise(RoadVertexDescriptor junctionVertex) const;

	std::string getInformation() const;

	void add(RoadVertexDescriptor source, RoadVertexDescriptor target);
	void add(sf::Vector2f sourcePos, RoadVertexDescriptor target);
	void add(RoadVertexDescriptor source, sf::Vector2f targetPos);
	void add(sf::Vector2f sourcePos, sf::Vector2f targetPos);
};


