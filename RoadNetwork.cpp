


#include "RoadNetwork.hpp"



void RoadNetwork::draw(sf::RenderWindow& window) const {

    // duplicated code, TODO: RoadRenderer
    for (const auto& road : roads) {

        std::array temp = { sf::Vertex{road.start}, sf::Vertex{road.end} };
        window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
    }
}

void RoadNetwork::add(const RoadElement& road)
{
    roads.push_back(road);
}


