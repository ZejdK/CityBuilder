


#pragma once
#include "RoadGraphTypes.hpp"
#include "CityView.hpp"
#include "RoadJunctionGeometry.hpp"
#include "Config.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/System/Vector2.hpp"
#include "RoadNetworkLayout.hpp"
#include "RoadSegmentGeometry.hpp"



class RoadRenderer {
    
    const ConfigGlobal &config;

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

    void renderLine(sf::RenderWindow& window, sf::Vector2f origin, sf::Vector2f destination);
    void renderEdge(sf::RenderWindow& window, RoadEdgeDescriptor edge, const RoadGraph& roadGraph);
    void renderVertex(sf::RenderWindow& window, RoadVertexDescriptor edge, const RoadGraph& roadGraph);

    void renderRoad(sf::RenderWindow& window, const RoadSegmentGeometry& re);
    
    void renderJunctionCenter(sf::RenderWindow& window, const RoadJunctionGeometry& roadSegmentJunction);
    void renderJunctionCrossings(sf::RenderWindow& window, const RoadJunctionGeometry& roadSegmentJunction);

public:
    RoadRenderer(const ConfigGlobal& config);

    void renderGraph(sf::RenderWindow& window, const RoadGraph& roadGraph);
    void renderRoads(sf::RenderWindow& window, const RoadNetworkLayout &roadLayout);
    void renderJunctions(sf::RenderWindow& window, const RoadNetworkLayout &roadLayout);

    void render(sf::RenderWindow& window, const RoadNetworkLayout &roadLayout, const RoadGraph &roadGraph, CityView cityView);
};


