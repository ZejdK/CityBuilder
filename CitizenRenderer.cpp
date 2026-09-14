


#include "CitizenRenderer.hpp"
#include "CitizenSimulation.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"
#include "Citizen.hpp"
#include <array>
#include "SFML/Graphics/PrimitiveType.hpp"
#include <string>
#include "SFML/Graphics/Texture.hpp"
#include "Math.hpp"
#include "CitizenJunctionCurve.hpp"
#include "RoadSegmentGeometry.hpp"
#include "CitizenLayoutContext.hpp"



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

std::array<sf::Vector2f, 4> CitizenRenderer::getVehicleVertices(sf::Vector2f pos, sf::Vector2f dir) {

	return {

	   pos + CB::Math::rotateInDirection(sf::Vector2f(20.f, 10.f), dir),   // top right
	   pos + CB::Math::rotateInDirection(sf::Vector2f(20.f, -10.f), dir),  // bottom right
	   pos + CB::Math::rotateInDirection(sf::Vector2f(-20.f, -10.f), dir), // bottom left
	   pos + CB::Math::rotateInDirection(sf::Vector2f(-20.f, 10.f), dir),  // top left
	};
}



CitizenRenderer::CitizenRenderer()
		: font("assetstemp/arial.ttf"), text(font), vehicleShape(10.f), indicatorShape(5.f), vehicleVertices(sf::PrimitiveType::Triangles, 6), states() {

	loadVehicleTextures();

	vehicleShape.setOrigin(sf::Vector2f(10.f, 10.f));
	vehicleShape.setFillColor(sf::Color(0, 155, 155, 155));
	indicatorShape.setOrigin(sf::Vector2f(5.f, 5.f));
	indicatorShape.setFillColor(sf::Color(255, 165, 0, 155));
}

void CitizenRenderer::render(sf::RenderWindow& window, const CitizenSimulation& citizenSimulation, float roadWidth) {

	for (auto &citizen : citizenSimulation.getCitizens()) {

		auto layoutContext{ citizenSimulation.getCitizenLayoutContext(citizen) };
		auto s{ citizen.getS() };

		if (!citizenSimulation.isCitizenOnFirstOrLastEdge(citizen.getId()) && citizen.isInsideJunction()) {

			auto curve{ CitizenJunctionCurve{ s, layoutContext, roadWidth } };
			auto [ citizenPos, citizenDir ] { curve.getCitizenPosAndDir(s) };

			renderVehicle(window, citizenPos, citizenDir, citizen.getColour());
			renderIndicators(window, citizen, layoutContext, citizenPos, citizenDir);
			// debugRenderJunctionCurveData(window, curve, citizenPos);
		}
		else {

			auto [ fromPos, toPos ] { citizenSimulation.getCitizenDirection(citizen.getId()) };
			auto offset = RoadSegmentGeometry::getLaneOffset(fromPos, toPos, roadWidth);
			auto citizenPos = lerp(fromPos + offset, toPos + offset, s);
			renderVehicle(window, citizenPos, toPos - fromPos, citizen.getColour());
			renderIndicators(window, citizen, layoutContext, citizenPos, toPos - fromPos);
		}
	}
}

void CitizenRenderer::renderIndicators(sf::RenderWindow &window, const Citizen &citizen, const CitizenLayoutContext& layoutContext, sf::Vector2f pos, sf::Vector2f dir) {

	auto v { getVehicleVertices(pos, dir) };

	if (citizen.isIndicatingLeft()) {

		indicatorShape.setPosition(v[1]);
		window.draw(indicatorShape);
		indicatorShape.setPosition(v[2]);
		window.draw(indicatorShape);
	}
	else if (citizen.isIndicatingRight()) {

		indicatorShape.setPosition(v[3]);
		window.draw(indicatorShape);
		indicatorShape.setPosition(v[0]);
		window.draw(indicatorShape);
	}
}

void CitizenRenderer::renderVehicle(sf::RenderWindow& window, sf::Vector2f pos, sf::Vector2f dir, std::string colour) {

	states.texture = &vehicleTextures[colour];

	auto v{ getVehicleVertices(pos, dir) };

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

void CitizenRenderer::debugRenderJunctionCurveData(sf::RenderWindow& window, const CitizenJunctionCurve &curve, sf::Vector2f citizenPos) {

	vehicleShape.setPosition(curve.entryPos);
	vehicleShape.setFillColor(sf::Color(255, 0, 255, 100));
	window.draw(vehicleShape);

	vehicleShape.setPosition(curve.entryControlPos);
	vehicleShape.setFillColor(sf::Color(255, 0, 255, 100));
	window.draw(vehicleShape);

	vehicleShape.setPosition(curve.exitPos);
	vehicleShape.setFillColor(sf::Color(0, 0, 255, 100));
	window.draw(vehicleShape);

	vehicleShape.setPosition(curve.exitControlPos);
	vehicleShape.setFillColor(sf::Color(0, 0, 255, 100));
	window.draw(vehicleShape);

	vehicleShape.setPosition(citizenPos);
	vehicleShape.setFillColor(sf::Color(255, 0, 180));
	window.draw(vehicleShape);
}


