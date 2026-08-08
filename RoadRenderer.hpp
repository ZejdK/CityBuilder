


#pragma once
#include <SFML/Graphics.hpp>
#include "RoadGraphTypes.hpp"
#include "RoadNetwork.hpp"
#include "CityView.hpp"
#include "RoadJunctionGeometry.hpp"



class RoadRenderer {
    
    sf::CircleShape graphNodeShape;
    sf::CircleShape debugShape;
    sf::ConvexShape junctionTriangle;
    sf::Font font;
    sf::Text text;

    sf::Texture texture;
    sf::Texture texturePlain;
    sf::Texture crossingTexture;
    sf::VertexArray vertices;
    sf::RenderStates states;
    static constexpr float ROAD_WIDTH{ 40.f };
    static constexpr float ROAD_CROSSING_SEGMENT_LENGTH { 128.f };

    void renderLine(sf::RenderWindow& window, sf::Vector2f origin, sf::Vector2f destination);
    void renderEdge(sf::RenderWindow& window, EdgeVertexDescriptor edge, const RoadGraph& roadGraph);

    void renderRoad(sf::RenderWindow& window, const RoadSegmentGeometry& re);
    
    void renderJunction(sf::RenderWindow& window, const RoadJunctionGeometry& roadSegmentJunction);
    void renderJunctionCrossings(sf::RenderWindow& window, const RoadJunctionGeometry& roadSegmentJunction, const std::vector<sf::Vector2f> &junctionPoints);
    void renderJunctionCenter(sf::RenderWindow& window, const RoadJunctionGeometry& roadSegmentJunction, const std::vector<sf::Vector2f> &junctionPoints);

public:
    RoadRenderer();

    void renderGraph(sf::RenderWindow& window, const RoadNetwork& roadNetwork);
    void renderRoads(sf::RenderWindow& window, const RoadNetwork& roadNetwork);
    void renderJunctions(sf::RenderWindow& window, const RoadNetwork& roadNetwork);

    void render(sf::RenderWindow& window, const RoadNetwork& roadNetwork, CityView cityView);
};


