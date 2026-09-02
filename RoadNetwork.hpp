


#pragma once
#include <vector>
#include "RoadGraphTypes.hpp"
#include "SFML/System/Vector2.hpp"
#include "RandomCycleFinder.hpp"
#include <utility>
#include "RoadNetworkLayout.hpp"
#include "boost/graph/detail/adjacency_list.hpp"
#include "Config.hpp"



class RoadNetwork {

	RoadGraph roadGraph;
	RoadNetworkLayout roadNetworkLayout;



	void addEdge(RoadVertexDescriptor source, RoadVertexDescriptor target);
	RoadVertexDescriptor addVertex(sf::Vector2f pos);

	void rebuildRoadLayout();

public:
	
	RoadNetwork(const ConfigGlobal &config);

	const RoadGraph& getGraph() const { return roadGraph; }
	const RoadNetworkLayout &getLayout() const { return roadNetworkLayout; }

	const std::vector<RoadEdgeDescriptor> getVertexOutEdges(RoadVertexDescriptor junction) const;

	std::pair<RoadVertexDescriptor, RoadVertexDescriptor> add(RoadVertexDescriptor source, RoadVertexDescriptor target);
	std::pair<RoadVertexDescriptor, RoadVertexDescriptor> add(sf::Vector2f sourcePos, RoadVertexDescriptor target);
	std::pair<RoadVertexDescriptor, RoadVertexDescriptor> add(RoadVertexDescriptor source, sf::Vector2f targetPos);
	std::pair<RoadVertexDescriptor, RoadVertexDescriptor> add(sf::Vector2f sourcePos, sf::Vector2f targetPos);

	std::vector<RoadVertexDescriptor> getRandomCycle() const {

		return RandomCycleFinder::getRandomCycle(roadGraph);
	}



	struct RoadNetworkInfo {

		unsigned long long vertexCount;
		unsigned long long edgeCount;
	};

	RoadNetworkInfo getInformation() const { return { boost::num_vertices(roadGraph), boost::num_edges(roadGraph) }; }
};


