


#include "CitizenSimulation.hpp"
#include <string>
#include <vector>
#include "Citizen.hpp"
#include "RoadGraphTypes.hpp"
#include "RoadNetwork.hpp"
#include "SFML/System/Vector2.hpp"
#include "CitizenPosition.hpp"
#include <array>



CitizenSimulation::CitizenSimulation(const RoadNetwork& roadNetwork)
	: roadNetwork(roadNetwork) {

	enabled = false;
}

void CitizenSimulation::addCitizen(int id, std::string name, std::string surname, std::vector<RoadVertexDescriptor> path, std::string colour) {

	citizens.push_back(Citizen(id, name, surname, path, colour));
}

const std::vector<Citizen>& CitizenSimulation::getCitizens() const {
	
	return citizens;
}

sf::Vector2f CitizenSimulation::getVertexPos(RoadVertexDescriptor vertex) const {

	return roadNetwork.getGraph()[vertex].position;
}

void CitizenSimulation::enableTest() {

	auto path { roadNetwork.getRandomCycle() };

	std::array<std::string, 5> colours { "b", "g", "w", "r", "o" };

	for (int i { 0 }; i < 50; ++i)
		addCitizen(i, "John", "Doe", roadNetwork.getRandomCycle(), colours[i % 5]);
}

void CitizenSimulation::update(float dt) {

	// temporary check for debugging/designing
	if (roadNetwork.getLayout().getInformation().junctionCount < 5)
		return;

	if (!enabled) {

		enableTest();
		enabled = true;
	}

	if (enabled) {
		
		for (auto &citizen : citizens) {
			
			// need to store the positional data in the citizen class and only update when the citizen moves to the next edge
			CitizenPosition posData { citizen.getPositionalData() };

			sf::Vector2f fromPos { getVertexPos(posData.from) };
			sf::Vector2f toPos { getVertexPos(posData.to) };

			bool finished { citizen.update(dt, (toPos - fromPos).length(), citizens) };
			if (finished) {

				enabled = false;
				break;
			}
		}
	}
}


