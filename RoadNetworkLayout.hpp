


#pragma once

#include <unordered_map>
#include "RoadJunctionGeometry.hpp"
#include "RoadSegmentGeometry.hpp"
#include "RoadGraphTypes.hpp"
#include "boost/graph/detail/edge.hpp"
#include <vector>
#include "SFML/System/Vector2.hpp"
#include "Config.hpp"



class RoadNetworkLayout {

	const ConfigGlobal &config;

	int junctionCount = 0;		// NOTE: trivial junctions (junctions with only two roads) are not counted
	int roadCount = 0;

	int roadIdCounter = 0;
	int junctionIdCounter = 0;
	
	std::unordered_map<RoadVertexDescriptor, RoadJunctionGeometry> junctions;
	std::unordered_map<RoadEdgeDescriptor, std::size_t, boost::hash<RoadEdgeDescriptor> > edgeToRoadIndex;
	std::vector<RoadSegmentGeometry> roads;

	friend class RoadNetwork;

	void setRoad(RoadEdgeDescriptor edge, RoadVertexDescriptor source, RoadVertexDescriptor target, sf::Vector2f sourcePos, sf::Vector2f targetPos);
	void setJunction(RoadVertexDescriptor vertex, int inDegree, sf::Vector2f position, const std::vector<RoadEdgeDescriptor> &edges);

public:

	RoadNetworkLayout(const ConfigGlobal &config);

	// should be a separate commit
	RoadNetworkLayout(const RoadNetworkLayout&) = delete;
	RoadNetworkLayout(RoadNetworkLayout&&) = default;
	RoadNetworkLayout& operator = (const RoadNetworkLayout &) = delete;
	RoadNetworkLayout& operator = (RoadNetworkLayout &&) = default;
	
	const RoadJunctionGeometry *getJunction(RoadVertexDescriptor vertex) const;
	const RoadSegmentGeometry *getRoad(RoadEdgeDescriptor edge) const;

	const std::vector<RoadSegmentGeometry> &getRoads() const { return roads; }
	const std::unordered_map<RoadVertexDescriptor, RoadJunctionGeometry> &getJunctions() const { return junctions; }

	void clear();

	const RoadSegmentGeometry *findHoveredRoad(float roadWidth, sf::Vector2f cursorPos) const;
	const RoadJunctionGeometry *findJunctionNear(float radius, sf::Vector2f position) const;



	struct RoadNetworkInfo {
		
		int junctionCount;
		int roadCount;
	};

	RoadNetworkInfo getInformation() const { return { junctionCount, roadCount }; }

	struct IntersectionResult {
		bool exists;
		sf::Vector2f point;
		bool snapped;
	};

	IntersectionResult doesLineIntersectAnyRoad(sf::Vector2f A, sf::Vector2f B, float snapRadius) const;
};


