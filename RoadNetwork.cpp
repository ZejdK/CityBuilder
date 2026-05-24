


#include "RoadNetwork.hpp"
#include <array>
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"



void RoadNetwork::draw(sf::RenderWindow& window) const {

    // duplicated code for rendering lines/roads
    for (const auto& road : roads) {

        std::array temp = { sf::Vertex{road.start}, sf::Vertex{road.end} };
        window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
    }
}

void RoadNetwork::add(const RoadElement& road)
{
    roads.push_back(road);
}


