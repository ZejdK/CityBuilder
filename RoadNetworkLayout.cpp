


#include "RoadNetworkLayout.hpp"
#include "RoadGraphTypes.hpp"
#include "RoadJunctionGeometry.hpp"
#include "RoadSegmentGeometry.hpp"
#include <utility>
#include <vector>
#include "SFML/System/Vector2.hpp"
#include <ranges>
#include "Math.hpp"
#include <optional>
#include "Config.hpp"



RoadNetworkLayout::RoadNetworkLayout(const ConfigGlobal &config)
	: junctionCount(0), roadCount(0), config(config) {}



const RoadJunctionGeometry *RoadNetworkLayout::getJunction(RoadVertexDescriptor vertex) const {
	
	auto it { junctions.find(vertex) };
	
	if (it == junctions.end())
		return nullptr;

	return &it->second;
}

const RoadSegmentGeometry *RoadNetworkLayout::getRoad(RoadEdgeDescriptor edge) const {
	
	auto it { edgeToRoadIndex.find(edge) };

	if (it == edgeToRoadIndex.end())
		return nullptr;

	return &roads[it->second];
}



void RoadNetworkLayout::setRoad(RoadEdgeDescriptor edge, RoadVertexDescriptor source, RoadVertexDescriptor target, sf::Vector2f sourcePos, sf::Vector2f targetPos) {

	// TODO: optimise so lookup is O(1)
	for (int i { 0 }; i < roads.size(); ++i)
		if (source == roads[i].getVertexB() && target == roads[i].getVertexA() ||
			source == roads[i].getVertexA() && target == roads[i].getVertexB()) {

			roads[i].addEdge(edge);
			edgeToRoadIndex.insert_or_assign(edge, i);
			return;
		}

	roads.push_back(RoadSegmentGeometry{ roadIdCounter++, source, target, sourcePos, targetPos });
	roads[roads.size() - 1].addEdge(edge);
	edgeToRoadIndex.insert_or_assign(edge, roads.size() - 1);

	++roadCount;
}

void RoadNetworkLayout::setJunction(RoadVertexDescriptor vertex, int inDegree, sf::Vector2f position, const std::vector<RoadEdgeDescriptor> &edges) {
	
	junctionCount += inDegree > 2;

	auto junctionRoadsView { edges | std::views::transform([this](RoadEdgeDescriptor e) { return getRoad(e); }) };

	std::vector<const RoadSegmentGeometry *> junctionRoads { junctionRoadsView.begin(), junctionRoadsView.end() };

	junctions.insert_or_assign(vertex, RoadJunctionGeometry { junctionIdCounter++, vertex, position, junctionRoads, config });
}



void RoadNetworkLayout::clear() {

	junctionCount = 0;
	roadCount = 0;

	roadIdCounter = 0;
	junctionIdCounter = 0;

	junctions.clear();
	edgeToRoadIndex.clear();
	roads.clear();
}



const RoadSegmentGeometry *RoadNetworkLayout::findHoveredRoad(float roadWidth, sf::Vector2f cursorPos) const {

	for (const auto &roadSegment : roads)
		if (roadSegment.isPointOnRoad(roadWidth, cursorPos))
			return &roadSegment;
	
	return nullptr;
}

const RoadJunctionGeometry *RoadNetworkLayout::findJunctionNear(float radius, sf::Vector2f position) const {

	for (const auto &[ vertex, junction ] : junctions)
		if (CB::Math::distance(junction.getPosition(), position) < radius)
			return &junction;

	return nullptr;
}



RoadNetworkLayout::IntersectionResult RoadNetworkLayout::doesLineIntersectAnyRoad(sf::Vector2f A, sf::Vector2f B, float snapRadius) const {

	std::optional<IntersectionResult> closestResult { std::nullopt };

	for (const auto& roadSegments : roads) {

		auto intersection { CB::Math::intersect(roadSegments.getStart(), roadSegments.getEnd(), A, B)};

		if (intersection) {

			float dist { CB::Math::distance(B, *intersection) };
			if (!closestResult) {

				closestResult = IntersectionResult{ true, *intersection, dist < snapRadius };
			}
			else {

				float distClosest { CB::Math::distance(B, closestResult->point) };
				if (dist < distClosest)
					closestResult = IntersectionResult{ true, *intersection, dist < snapRadius };
			}
		}
	}

	if (closestResult)
		return *closestResult;

	return { false, sf::Vector2f(0, 0), false };
}


