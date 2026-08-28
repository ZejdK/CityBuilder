


#pragma once
#include "Citizen.hpp"
#include <string>
#include <vector>
#include "RoadGraphTypes.hpp"
#include "RoadNetwork.hpp"
#include "SFML/System/Vector2.hpp"



class CitizenSimulation
{
    std::vector<Citizen> citizens;
    const RoadNetwork &roadNetwork;
    bool enabled;

public:

    CitizenSimulation(const RoadNetwork& roadNetwork);

    void addCitizen(int id, std::string name, std::string surname, std::vector<RoadVertexDescriptor> path, std::string colour);

    const std::vector<Citizen> &getCitizens() const;
    sf::Vector2f getVertexPos(RoadVertexDescriptor vertex) const;

    void enableTest();

    void update(float tNew);
};


