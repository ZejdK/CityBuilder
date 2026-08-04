


#pragma once
#include <SFML/Graphics.hpp>
#include "RoadGraphTypes.hpp"
#include "RoadNetwork.hpp"
#include "CityView.hpp"



class RoadRenderer {
    
    sf::CircleShape graphNodeShape;
    sf::Font font;
    sf::Text text;

    sf::Texture texture;
    sf::Texture texturePlain;
    sf::VertexArray vertices;
    sf::RenderStates states;
    static constexpr float ROAD_WIDTH{ 40.f };

    void renderLine(sf::RenderWindow& window, sf::Vector2f origin, sf::Vector2f destination);
    void renderEdge(sf::RenderWindow& window, EdgeVertexDescriptor edge, const RoadGraph& roadGraph);

    void renderRoad(sf::RenderWindow& window, const RoadElement& re);
    
    void renderIntersection(sf::RenderWindow& window, const RoadGraph& roadGraph, RoadVertexDescriptor vertex);

public:
    RoadRenderer();

    void renderGraph(sf::RenderWindow& window, const RoadNetwork& roadNetwork);
    void renderRoads(sf::RenderWindow& window, const RoadNetwork& roadNetwork);
    void renderIntersections(sf::RenderWindow& window, const RoadNetwork& roadNetwork);

    void render(sf::RenderWindow& window, const RoadNetwork& roadNetwork, CityView cityView);
};


