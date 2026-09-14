


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
#include "CitizenJunctionCurve.hpp"
#include <array>
#include "Citizen.hpp"
#include "CitizenLayoutContext.hpp"



class CitizenRenderer {

    sf::Font font;
    sf::Text text;

    sf::CircleShape vehicleShape;
    sf::CircleShape indicatorShape;
    sf::VertexArray vehicleVertices;
	std::unordered_map<std::string, sf::Texture> vehicleTextures;
    sf::RenderStates states;



	void loadVehicleTextures();

    std::array<sf::Vector2f, 4> getVehicleVertices(sf::Vector2f pos, sf::Vector2f dir);

    void renderVehicle(sf::RenderWindow& window, sf::Vector2f pos, sf::Vector2f dir, std::string colour);
    void renderIndicators(sf::RenderWindow& window, const Citizen &citizen, const CitizenLayoutContext& layoutContext, sf::Vector2f pos, sf::Vector2f dir);
    void debugRenderJunctionCurveData(sf::RenderWindow& window, const CitizenJunctionCurve &curve, sf::Vector2f citizenPos);

public:

    CitizenRenderer();

    void render(sf::RenderWindow& window, const CitizenSimulation &citizenSimulation, float roadWidth);
};


