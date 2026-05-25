


#include "RoadNetwork.hpp"
#include <array>
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "Math.hpp"



RoadNetwork::RoadNetwork() : graphNodeShape(15.f), font("assetstemp/arial.ttf"), text(font) {

	text.setPosition(sf::Vector2f(10.f, 10.f));
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::Cyan);

	graphNodeShape.setOrigin(sf::Vector2f(15.f, 15.f));
	graphNodeShape.setFillColor(sf::Color::Yellow);
}

void RoadNetwork::draw(sf::RenderWindow& window) const {

    // duplicated code for rendering lines/roads
    for (const auto& road : roads) {

        std::array temp = { sf::Vertex{road.start}, sf::Vertex{road.end} };
        window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
    }
}

void RoadNetwork::drawGraph(sf::RenderWindow& window) {

	auto [ begin, end ] = boost::edges(roadGraph);

	for (; begin != end; ++begin) {

		auto edge = *begin;

		auto sourceVertex = boost::source(edge, roadGraph);
		auto targetVertex = boost::target(edge, roadGraph);

		const RoadNode& sourceNode = roadGraph[sourceVertex];
		const RoadNode& targetNode = roadGraph[targetVertex];

		const RoadEdge& edgeData = roadGraph[edge];

		graphNodeShape.setPosition(sourceNode.position);
		window.draw(graphNodeShape);

		graphNodeShape.setPosition(targetNode.position);
		window.draw(graphNodeShape);

		// duplicated code for rendering lines/roads
		std::array temp = { sf::Vertex{sourceNode.position}, sf::Vertex{targetNode.position} };
		window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);

		sf::Vector2f middle { (sourceNode.position + targetNode.position) / 2.f };
		text.setPosition(middle);
		text.setString(std::to_string(edgeData.length));
		window.draw(text);
	}
}

void RoadNetwork::add(const RoadElement& road)
{
    roads.push_back(road);

	auto node1 = boost::add_vertex(roadGraph);
	auto node2 = boost::add_vertex(roadGraph);
	roadGraph[node1].position = road.start;
	roadGraph[node2].position = road.end;

	auto [ edge1, inserted1 ] = boost::add_edge(node1, node2, roadGraph);
	roadGraph[edge1].length = CB::Math::distance(road.start, road.end);
	roadGraph[edge1].speedLimit = 50.f;

	auto [ edge2, inserted2 ] = boost::add_edge(node2, node1, roadGraph);
	roadGraph[edge2].length = roadGraph[edge1].length;
	roadGraph[edge2].speedLimit = 50.f;
}


