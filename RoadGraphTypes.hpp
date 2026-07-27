


#pragma once
#include <SFML/System/Vector2.hpp>
#include <boost/graph/adjacency_list.hpp>



struct RoadNode {

	sf::Vector2f position;
};

struct RoadEdge {

	float length;
	float speedLimit;
};

using RoadGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, RoadNode, RoadEdge>;
//									    edge container, vertex container, graph direction, vertex payload, edge payload
// BGL uses STL containers to represent the set of vertices and the adjacency structure(out - edges and in - edges) of the graph.
// There are several selector types that are used to specify the choice of container for OutEdgeList and VertexList.
//		vecS selects std::vector.

using RoadVertexDescriptor = boost::graph_traits<RoadGraph>::vertex_descriptor;
using EdgeVertexDescriptor = boost::graph_traits<RoadGraph>::edge_descriptor;


