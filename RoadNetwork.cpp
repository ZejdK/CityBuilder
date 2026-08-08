


#include "RoadNetwork.hpp"
#include "Math.hpp"
#include <algorithm>
#include "RoadSegmentGeometry.hpp"



RoadNetwork::RoadNetwork() {}

const RoadGraph& RoadNetwork::getGraph() const {

	return roadGraph;
}

const std::vector<RoadSegmentGeometry>& RoadNetwork::getRoads() const {

const std::optional<RoadVertexDescriptor> RoadNetwork::findJunctionNear(float radius, sf::Vector2f position) const {

	auto [begin, end] = boost::vertices(roadGraph);

	for (auto it{ begin }; it != end; ++it) {

		RoadVertexDescriptor vertex { *it };

		if (CB::Math::distance(roadGraph[vertex].position, position) < radius)
			return vertex;
	}

	return std::nullopt;
}

std::optional<sf::Vector2f> RoadNetwork::findJunctionPosNear(float radius, sf::Vector2f position) const {

	std::optional<RoadVertexDescriptor> closestVertex = findJunctionNear(radius, position);

	if (closestVertex)
		return roadGraph[*closestVertex].position;

	return std::nullopt;
}



std::vector<RoadSegmentGeometry> RoadNetwork::getJunctionRoadsClockwise(RoadVertexDescriptor junctionVertex) const
{
	std::vector<RoadSegmentGeometry> roads;
	sf::Vector2f junctionPos { roadGraph[junctionVertex].position };

	auto [ beginOut, endOut ] = boost::out_edges(junctionVertex, roadGraph);
	for (auto it { beginOut }; it != endOut; ++it) {

		EdgeVertexDescriptor edge = *it;
		auto targetVertex = boost::target(edge, roadGraph);
		roads.push_back({ roadGraph[junctionVertex].position, roadGraph[targetVertex].position });
	}

	//auto [beginIn, endIn] = boost::in_edges(junctionVertex, roadGraph);
	//for (auto it { beginIn }; it != endIn; ++it) {

	//	EdgeVertexDescriptor edge = *it;
	//	auto sourceVertex = boost::source(edge, roadGraph);
	//	roads.push_back({ roadGraph[junctionVertex].position, roadGraph[sourceVertex].position });
	//}

	std::sort(roads.begin(), roads.end(), [](const RoadSegmentGeometry& a, const RoadSegmentGeometry& b) {

		float angleA = std::atan2(a.end.y - a.start.y, a.end.x - a.start.x);
		float angleB = std::atan2(b.end.y - b.start.y, b.end.x - b.start.x);

		return angleA > angleB;
	});

	return roads;
}

void RoadNetwork::add(const RoadSegmentGeometry& road)
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


