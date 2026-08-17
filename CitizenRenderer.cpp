


#include "CitizenRenderer.hpp"
#include "CitizenSimulation.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"
#include "Citizen.hpp"
#include "CitizenPosition.hpp"
#include <array>
#include "SFML/Graphics/PrimitiveType.hpp"
#include <string>
#include "SFML/Graphics/Texture.hpp"
#include "Math.hpp"



void CitizenRenderer::loadVehicleTextures() {

	std::array<std::string, 5> colours{ "b", "g", "w", "r", "o" };

	for (auto& colour : colours) {

		sf::Texture texture;

		if (texture.loadFromFile("assets/vehicle_" + colour + ".png"))
			vehicleTextures[colour] = texture;
		else
			throw "Unable to load the texture: vehicle_" + colour + ".png";
	}
}

sf::Vector2f CitizenRenderer::lerp(sf::Vector2f A, sf::Vector2f B, float t) const {
	
	return A + (B - A) * t;
}

sf::Vector2f CitizenRenderer::laneOffset(sf::Vector2f fromPos, sf::Vector2f toPos) const {

	float offset { 20.f }; /// SHOULD BE ROAD_WIDTH / 2

	// assumes points will not overlap
	auto dir = (toPos - fromPos).normalized();
	auto normal = sf::Vector2f(-dir.y, dir.x);

	return normal * offset;
}



CitizenRenderer::CitizenRenderer()
		: font("assetstemp/arial.ttf"), text(font), vehicleShape(10.f), vehicleVertices(sf::PrimitiveType::Triangles, 6), states() {

	loadVehicleTextures();

	vehicleShape.setOrigin(sf::Vector2f(10.f, 10.f));
	vehicleShape.setFillColor(sf::Color(0, 155, 155, 155));
}

void CitizenRenderer::render(sf::RenderWindow& window, const CitizenSimulation& citizenSimulation) {

	for (auto &citizen : citizenSimulation.getCitizens()) {

		CitizenPosition posData { citizen.getPositionalData() };

		sf::Vector2f fromPos { citizenSimulation.getVertexPos(posData.from) };
		sf::Vector2f toPos { citizenSimulation.getVertexPos(posData.to) };

		auto offset = laneOffset(fromPos, toPos);
		auto citizenPos = lerp(fromPos + offset, toPos + offset, posData.s);

		renderVehicle(window, citizenPos, toPos - fromPos, citizen.getColour());
	}
}

void CitizenRenderer::renderVehicle(sf::RenderWindow& window, sf::Vector2f pos, sf::Vector2f dir, std::string colour) {

	states.texture = &vehicleTextures[colour];

	std::array<sf::Vector2f, 4> v {

		pos + CB::Math::rotateInDirection(sf::Vector2f(20.f, 10.f), dir),   // top right
		pos + CB::Math::rotateInDirection(sf::Vector2f(20.f, -10.f), dir),  // bottom right
		pos + CB::Math::rotateInDirection(sf::Vector2f(-20.f, -10.f), dir), // bottom left
		pos + CB::Math::rotateInDirection(sf::Vector2f(-20.f, 10.f), dir),  // top left
	};

	vehicleVertices[0].position = v[0];
	vehicleVertices[1].position = v[1];
	vehicleVertices[2].position = v[2];
	vehicleVertices[0].texCoords = { 128.f, 0.f };
	vehicleVertices[1].texCoords = { 128.f, 64.f };
	vehicleVertices[2].texCoords = { 0.f, 64.f };

	vehicleVertices[3].position = v[0];
	vehicleVertices[4].position = v[2];
	vehicleVertices[5].position = v[3];
	vehicleVertices[3].texCoords = { 128.f, 0.f };
	vehicleVertices[4].texCoords = { 0.f, 64.f };
	vehicleVertices[5].texCoords = { 0.f, 0.f };

	window.draw(vehicleVertices, states);
}


