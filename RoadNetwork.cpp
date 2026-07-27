


#include "RoadNetwork.hpp"
#include "Math.hpp"



RoadNetwork::RoadNetwork() {}

const RoadGraph& RoadNetwork::getGraph() const {

	return roadGraph;
}

const std::vector<RoadElement>& RoadNetwork::getRoads() const {

	return roads;
}

void RoadNetwork::add(const RoadElement& road)
{
    roads.push_back(road);

	RoadVertexDescriptor nodeOrigin = findOrCreateVertex(road.start);
	RoadVertexDescriptor nodeDest = findOrCreateVertex(road.end);

	auto [ edge1, inserted1 ] = boost::add_edge(nodeOrigin, nodeDest, roadGraph);
	roadGraph[edge1].length = CB::Math::distance(road.start, road.end);
	roadGraph[edge1].speedLimit = 50.f;

	auto [ edge2, inserted2 ] = boost::add_edge(nodeDest, nodeOrigin, roadGraph);
	roadGraph[edge2].length = roadGraph[edge1].length;
	roadGraph[edge2].speedLimit = 50.f;
}

RoadVertexDescriptor RoadNetwork::findOrCreateVertex(sf::Vector2f pos) {

	if (auto node { findVertexAtPosition(pos) })
		return *node;

	auto newNode = boost::add_vertex(roadGraph);
	roadGraph[newNode].position = pos;
	
	return newNode;
}

std::optional<RoadVertexDescriptor> RoadNetwork::findVertexAtPosition(sf::Vector2f pos) {

	auto [ begin, end ] = boost::vertices(roadGraph);

	for (auto it { begin }; it != end; ++it) {

		RoadVertexDescriptor vertex { *it };

		if (CB::Math::equals(pos, roadGraph[vertex].position))
			return vertex;
	}

	return std::nullopt;
}


