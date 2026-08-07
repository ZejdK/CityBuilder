


#pragma once
#include <vector>
#include "RoadTypes.hpp"
#include "RoadGraphTypes.hpp"
#include <vector>



class RoadNetwork {

	RoadGraph roadGraph;
	std::vector<RoadElement> roads{};

	RoadVertexDescriptor findOrCreateVertex(sf::Vector2f pos);
	std::optional<RoadVertexDescriptor> findVertexAtPosition(sf::Vector2f pos);

	// TODO: clean up added extra nodes, see line below
	//			if i connect an edge to "middle of another edge". i would need to split edge into two edges to create a node to connect to the newly added edge. If i remove this newly added edge, I would then be left with a "trivial" node, ie node with only two edges and is also in the middle of a geometric line. periodically go through all the nodes to clean these up

public:
	
	RoadNetwork();

	const RoadGraph& getGraph() const;
	const std::vector<RoadElement>& getRoads() const;

	std::vector<RoadElement> getJunctionRoadsClockwise(RoadVertexDescriptor junctionVertex) const;

	void add(const RoadElement& road);
};


