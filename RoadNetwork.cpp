


#include "RoadNetwork.hpp"
#include "Math.hpp"
#include <vector>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/detail/adjacency_list.hpp"
#include "SFML/System/Vector2.hpp"
#include "RoadGraphTypes.hpp"
#include <utility>
#include "Config.hpp"



RoadNetwork::RoadNetwork(const ConfigGlobal &config)
	: roadNetworkLayout(config) {}



const std::vector<RoadEdgeDescriptor> RoadNetwork::getVertexOutEdges(RoadVertexDescriptor junction) const {
	
	std::vector<RoadEdgeDescriptor> edges;

	auto [ begin, end ] = boost::out_edges(junction, roadGraph);
	for (auto it { begin }; it != end; ++it)
		edges.push_back(*it);

	return edges;
}

std::pair<RoadVertexDescriptor, RoadVertexDescriptor> RoadNetwork::getEdgeVertices(RoadEdgeDescriptor edge) const {

	return { boost::source(edge, roadGraph), boost::target(edge, roadGraph) };
}


void RoadNetwork::addEdge(RoadVertexDescriptor source, RoadVertexDescriptor target) {

	float length{ CB::Math::distance(roadGraph[source].position, roadGraph[target].position) };

	auto [ edge1, inserted1 ] = boost::add_edge(source, target, roadGraph);
	roadGraph[edge1].length = length;
	roadGraph[edge1].speedLimit = 50.f;

	auto [ edge2, inserted2 ] = boost::add_edge(target, source, roadGraph);
	roadGraph[edge2].length = length;
	roadGraph[edge2].speedLimit = 50.f;

	rebuildRoadLayout();
}

RoadVertexDescriptor RoadNetwork::addVertex(sf::Vector2f pos) {

	auto newVertex = boost::add_vertex(roadGraph);
	roadGraph[newVertex].position = pos;

	return newVertex;
}

std::pair<RoadVertexDescriptor, RoadVertexDescriptor> RoadNetwork::add(RoadVertexDescriptor source, RoadVertexDescriptor target) {

	addEdge(source, target);
	
	return { source, target };
}

std::pair<RoadVertexDescriptor, RoadVertexDescriptor> RoadNetwork::add(sf::Vector2f sourcePos, RoadVertexDescriptor target) {

	auto sourceNew = addVertex(sourcePos);
	addEdge(sourceNew, target);
	
	return { sourceNew, target };
}
std::pair<RoadVertexDescriptor, RoadVertexDescriptor> RoadNetwork::add(RoadVertexDescriptor source, sf::Vector2f targetPos) {

	auto targetNew = addVertex(targetPos);
	addEdge(source, targetNew);
	
	return { source, targetNew };
}

std::pair<RoadVertexDescriptor, RoadVertexDescriptor> RoadNetwork::add(sf::Vector2f sourcePos, sf::Vector2f targetPos) {

	auto sourceNew = addVertex(sourcePos);
	auto targetNew = addVertex(targetPos);
	addEdge(sourceNew, targetNew);
	
	return { sourceNew, targetNew };
}



void RoadNetwork::rebuildRoadLayout() {
	
	roadNetworkLayout.clear();

	// edges need to be set first because getJunctionRoadsClockwise needs the edges pointers

	auto [ ebegin, eend ] = boost::edges(roadGraph);
	for (auto it { ebegin }; it != eend; ++it) {

		auto sourceVertex { boost::source(*it, roadGraph) };
		auto targetVertex { boost::target(*it, roadGraph) };

		roadNetworkLayout.setRoad(*it, sourceVertex, targetVertex, roadGraph[sourceVertex].position, roadGraph[targetVertex].position);
	}

	auto [ vbegin, vend ] = boost::vertices(roadGraph);
	for (auto it { vbegin }; it != vend; ++it)
		roadNetworkLayout.setJunction(*it, int(boost::in_degree(*it, roadGraph)), roadGraph[*it].position, getVertexOutEdges(*it));
}


