


#pragma once
#include "CitizenSimulation.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include <string>
#include <unordered_map>



class CitizenRenderer {

    sf::Font font;
    sf::Text text;

    sf::CircleShape vehicleShape;
    sf::VertexArray vehicleVertices;
	std::unordered_map<std::string, sf::Texture> vehicleTextures;
    sf::RenderStates states;

    // probably should move these into globalConfig
    // static constexpr float ROAD_WIDTH{ 40.f };
    // static constexpr float ROAD_CROSSING_SEGMENT_LENGTH{ 128.f };

	void loadVehicleTextures();

    sf::Vector2f lerp(sf::Vector2f A, sf::Vector2f B, float t) const;
    sf::Vector2f laneOffset(sf::Vector2f fromPos, sf::Vector2f toPos) const;

	void renderVehicle(sf::RenderWindow& window, sf::Vector2f pos, sf::Vector2f dir, std::string colour);

public:

    CitizenRenderer();

    void render(sf::RenderWindow& window, const CitizenSimulation &citizenSimulation);
};


