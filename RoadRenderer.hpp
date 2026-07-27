


#pragma once
#include <SFML/Graphics.hpp>
#include "RoadGraphTypes.hpp"
#include "RoadNetwork.hpp"



class RoadRenderer {
    
    sf::CircleShape graphNodeShape;
    sf::Font font;
    sf::Text text;

    sf::Texture texture;
    sf::Texture texturePlain;
    sf::VertexArray vertices;
    sf::RenderStates states;
    static constexpr float ROAD_WIDTH{ 40.f };

    std::array<sf::Vector2f, 4> getRoadVertices(sf::Vector2f source, sf::Vector2f dest);

    void renderLine(sf::RenderWindow& window, sf::Vector2f origin, sf::Vector2f destination);
    void renderEdge(sf::RenderWindow& window, EdgeVertexDescriptor edge, const RoadGraph& roadGraph);

    void renderRoad(sf::RenderWindow& window, sf::Vector2f origin, sf::Vector2f dest);
    
    void processIntersectionVertex(sf::RenderWindow& window, const RoadGraph& roadGraph, RoadVertexDescriptor vertex);

public:
    RoadRenderer();

    void renderGraph(sf::RenderWindow& window, const RoadNetwork& roadNetwork);
    void renderRoads(sf::RenderWindow& window, const RoadNetwork& roadNetwork);
    void renderIntersections(sf::RenderWindow& window, const RoadNetwork& roadNetwork);
};


