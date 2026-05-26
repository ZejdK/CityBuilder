


#pragma once
#include <SFML/Graphics.hpp>



class RoadRenderer {
    
    sf::Texture texture;
    sf::VertexArray vertices;
    sf::RenderStates states;
    static constexpr float ROAD_WIDTH{ 40.f };

public:
    RoadRenderer();

    void render(sf::RenderWindow& window, sf::Vector2f origin, sf::Vector2f dest);
};


