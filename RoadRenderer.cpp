


#include "RoadRenderer.hpp"
#include <SFML/System/Vector2.hpp>




RoadRenderer::RoadRenderer()
        : vertices(sf::PrimitiveType::Triangles, 6), states() {

    if (!texture.loadFromFile("assets/road_dotted.png"))
        throw "Unable to load the texture";

    texture.setRepeated(true);
    states.texture = &texture;
}

void RoadRenderer::render(sf::RenderWindow& window, sf::Vector2f origin, sf::Vector2f dest) {

    sf::Vector2f direction { (dest - origin).normalized() };
    sf::Vector2f perpendicular { -direction.y, direction.x }; // multiplied with rotation matrix for pi/2

    sf::Vector2f offset { perpendicular * ROAD_WIDTH };

    sf::Vector2f v0 = origin + offset;
    sf::Vector2f v1 = dest + offset;
    sf::Vector2f v2 = dest - offset;
    sf::Vector2f v3 = origin - offset;
    
    const float len { (dest - origin).length() };
    vertices[0].position = v0;
    vertices[1].position = v1;
    vertices[2].position = v2;
    vertices[3].position = v0;
    vertices[4].position = v2;
    vertices[5].position = v3;
    vertices[0].texCoords = { 0.f, 0.f };
    vertices[1].texCoords = { 0.f, len };
    vertices[2].texCoords = { 256.f, len };
    vertices[3].texCoords = { 0.f, 0.f };
    vertices[4].texCoords = { 256.f, len };
    vertices[5].texCoords = { 256.f, 0.f };

    window.draw(vertices, states);
}


