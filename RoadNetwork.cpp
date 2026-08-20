


#include "RoadNetwork.hpp"
#include "Math.hpp"
#include <algorithm>
#include "RoadSegmentGeometry.hpp"
#include <cmath>
#include <optional>
#include <unordered_set>
#include <vector>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/detail/adjacency_list.hpp"
#include "SFML/System/Vector2.hpp"
#include "RoadGraphTypes.hpp"
#include <string>
#include <format>



RoadNetwork::RoadNetwork() {}

const RoadGraph& RoadNetwork::getGraph() const {

	return roadGraph;
}

const std::vector<RoadVertexDescriptor> RoadNetwork::getJunctions() const {
	
	std::vector<RoadVertexDescriptor> junctions;
	auto [ begin, end ] = boost::vertices(roadGraph);

	for (auto it { begin }; it != end; ++it)
		junctions.push_back(*it);

	return junctions;
}

const std::optional<RoadVertexDescriptor> RoadNetwork::findJunctionNear(float radius, sf::Vector2f position) const {

	auto [begin, end] = boost::vertices(roadGraph);

	for (auto it{ begin }; it != end; ++it) {

		RoadVertexDescriptor vertex { *it };

		if (CB::Math::distance(roadGraph[vertex].position, position) < radius)
			return vertex;
	}

	return std::nullopt;
}

const std::optional<RoadEdgeDescriptor> RoadNetwork::findHoveredRoad(float roadWidth, sf::Vector2f cursorPos) const {

	auto [ begin, end ] = boost::edges(roadGraph);
	
	for (auto it { begin }; it != end; ++it) {
		
		RoadEdgeDescriptor edge { *it };

		RoadVertexDescriptor sourceVertex { boost::source(edge, roadGraph) };
		RoadVertexDescriptor targetVertex { boost::target(edge, roadGraph) };

		RoadSegmentGeometry roadSegment { roadGraph[sourceVertex].position, roadGraph[targetVertex].position };
		if (roadSegment.isPointOnRoad(roadWidth, cursorPos))
			return edge;
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

	std::unordered_set<RoadVertexDescriptor> seen;

	auto addRoad = [&](RoadVertexDescriptor otherVertex) {

		if (seen.insert(otherVertex).second) // if the item was inserted into the set, .insert returns std::pair, of which the second value is bool that indicates this
			roads.push_back({ roadGraph[junctionVertex].position, roadGraph[otherVertex].position });
	};

	auto [ beginOut, endOut ] = boost::out_edges(junctionVertex, roadGraph);
	for (auto edgeIt { beginOut }; edgeIt != endOut; ++edgeIt)
		addRoad(boost::target(*edgeIt, roadGraph));

	auto [ beginIn, endIn ] = boost::in_edges(junctionVertex, roadGraph);
	for (auto edgeIt { beginIn }; edgeIt != endIn; ++edgeIt)
		addRoad(boost::source(*edgeIt, roadGraph));

	std::sort(roads.begin(), roads.end(), [](const RoadSegmentGeometry& a, const RoadSegmentGeometry& b) {

		float angleA = std::atan2(a.end.y - a.start.y, a.end.x - a.start.x);
		float angleB = std::atan2(b.end.y - b.start.y, b.end.x - b.start.x);

		return angleA > angleB;
	});

	return roads;
}

std::string RoadNetwork::getInformation() const
{
	// this information should be stored in the class fields instead of iterating every frame
	// roadCount will break when one way roads are introduced, but this data should be cached anyway
	// TODO: store road network stats values into fields

	int junctionCount { getJunctionCount() };
	int roadCount{ int(boost::num_edges(roadGraph)) / 2 };

	return std::format("vertices:{}, edges:{}, junctions:{}, roads:{}", boost::num_vertices(roadGraph), boost::num_edges(roadGraph), junctionCount, roadCount);
}

int RoadNetwork::getJunctionCount() const {

	// TODO: this information should be stored in a field rather than calculated every time
	int junctionCount { 0 };

	auto [begin, end] = boost::vertices(roadGraph);
	for (auto it{ begin }; it != end; ++it)
		if (boost::in_degree(*it, roadGraph) > 2)
			++junctionCount;
	
	return junctionCount;
}



void RoadNetwork::addEdge(RoadVertexDescriptor source, RoadVertexDescriptor target) {

	float length{ CB::Math::distance(roadGraph[source].position, roadGraph[target].position) };

	auto [ edge1, inserted1 ] = boost::add_edge(source, target, roadGraph);
	roadGraph[edge1].length = length;
	roadGraph[edge1].speedLimit = 50.f;

	auto [ edge2, inserted2 ] = boost::add_edge(target, source, roadGraph);
	roadGraph[edge2].length = length;
	roadGraph[edge2].speedLimit = 50.f;
}

RoadVertexDescriptor RoadNetwork::addVertex(sf::Vector2f pos) {

	auto newVertex = boost::add_vertex(roadGraph);
	roadGraph[newVertex].position = pos;

	return newVertex;
}

void RoadNetwork::add(RoadVertexDescriptor source, RoadVertexDescriptor target) {

	addEdge(source, target);
}

void RoadNetwork::add(sf::Vector2f sourcePos, RoadVertexDescriptor target) {

	auto sourceNew = addVertex(sourcePos);
	addEdge(sourceNew, target);
}
void RoadNetwork::add(RoadVertexDescriptor source, sf::Vector2f targetPos) {

	auto targetNew = addVertex(targetPos);
	addEdge(source, targetNew);
}

void RoadNetwork::add(sf::Vector2f sourcePos, sf::Vector2f targetPos) {

	auto sourceNew = addVertex(sourcePos);
	auto targetNew = addVertex(targetPos);
	addEdge(sourceNew, targetNew);
}


