


#include "RoadNetwork.hpp"
#include <array>
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "Math.hpp"



RoadNetwork::RoadNetwork() : graphNodeShape(15.f), font("assetstemp/arial.ttf"), text(font) {

	text.setPosition(sf::Vector2f(10.f, 10.f));
	text.setCharacterSize(24);
	text.setFillColor(sf::Color(220, 170, 180));

	graphNodeShape.setOrigin(sf::Vector2f(15.f, 15.f));
	graphNodeShape.setFillColor(sf::Color(140, 0, 50));
}

void RoadNetwork::debugDrawLine(sf::RenderWindow& window, sf::Vector2f origin, sf::Vector2f destination) {

	std::array temp = { sf::Vertex{origin}, sf::Vertex{destination} };
	window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
}

void RoadNetwork::draw(sf::RenderWindow& window) {

    for (const auto& road : roads)
		debugDrawLine(window, road.start, road.end);
}

// TODO: edges are rendered twice
void RoadNetwork::drawGraph(sf::RenderWindow& window) {

	auto [ begin, end ] = boost::edges(roadGraph);

	for (; begin != end; ++begin) {

		auto edge = *begin;

		auto sourceVertex = boost::source(edge, roadGraph);
		auto targetVertex = boost::target(edge, roadGraph);

		const RoadNode& sourceNode = roadGraph[sourceVertex];
		const RoadNode& targetNode = roadGraph[targetVertex];

		const RoadEdge& edgeData = roadGraph[edge];

		const auto sourceDegree = boost::out_degree(sourceVertex, roadGraph);
		const auto targetDegree = boost::out_degree(targetVertex, roadGraph);

		roadRenderer.render(window, sourceNode.position, targetNode.position);

		graphNodeShape.setPosition(sourceNode.position);
		text.setPosition(sourceNode.position);
		text.setString(std::to_string(sourceDegree));
		window.draw(graphNodeShape);
		window.draw(text);

		text.setPosition(targetNode.position);
		text.setString(std::to_string(targetDegree));
		graphNodeShape.setPosition(targetNode.position);
		window.draw(graphNodeShape);
		window.draw(text);

		debugDrawLine(window, sourceNode.position, targetNode.position);

		sf::Vector2f middle { (sourceNode.position + targetNode.position) / 2.f };
		text.setPosition(middle);
		text.setString(std::to_string(edgeData.length));
		window.draw(text);
	}
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


