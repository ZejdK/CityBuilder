


#include "RoadNetwork.hpp"
#include "Math.hpp"
#include <vector>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/detail/adjacency_list.hpp"
#include "SFML/System/Vector2.hpp"
#include "RoadGraphTypes.hpp"
#include <utility>
#include "Config.hpp"
#include "boost/property_map/property_map.hpp"
#include "boost/graph/properties.hpp"
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <algorithm>
#include "boost/graph/named_function_params.hpp"
#include <stdexcept>



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

// these should return RoadNodeData pair
std::pair<sf::Vector2f, sf::Vector2f> RoadNetwork::getEdgeInitialPosDir(RoadEdgeDescriptor edge) const {

	auto [ source, target ] { getEdgeVertices(edge) };

	auto initPos{ roadGraph[source].position };
	auto initDir{ (roadGraph[target].position - roadGraph[source].position).normalized() };

	return { initPos, initDir };
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

std::vector<RoadEdgeDescriptor> RoadNetwork::getShortestPath(RoadVertexDescriptor source, RoadVertexDescriptor target) const {

	const auto vertexCount = boost::num_vertices(roadGraph);
	std::vector<float> distances(vertexCount);
	std::vector<RoadVertexDescriptor> predecessors(vertexCount);

	auto indexMap = boost::get(boost::vertex_index, roadGraph);
	auto distanceMap = boost::make_iterator_property_map(distances.begin(), indexMap);
	auto predecessorMap = boost::make_iterator_property_map(predecessors.begin(), indexMap);

	boost::dijkstra_shortest_paths(roadGraph, source, boost::weight_map(boost::get(&RoadEdgeData::length, roadGraph))
																.distance_map(distanceMap)
																.predecessor_map(predecessorMap));
	std::vector<RoadEdgeDescriptor> path;

	auto current = target;
	while (current != source) {

		auto previous = predecessors[current];
		auto [ edge, found ] = boost::edge(previous, current, roadGraph);

		if (!found)
			throw std::runtime_error("Error reconstructing dijkstra path!");

		path.push_back(edge);
		current = previous;
	}

	std::reverse(path.begin(), path.end());

	return path;
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


