


#pragma once
#include "Citizen.hpp"
#include <vector>
#include "RoadGraphTypes.hpp"
#include "RoadNetwork.hpp"
#include "SFML/System/Vector2.hpp"
#include "VehiclePath.hpp"
#include <stdexcept>
#include <utility>
#include "CitizenLayoutContext.hpp"
#include "RoadJunctionGeometry.hpp"
#include "RoadSegmentGeometry.hpp"



class CitizenSimulation
{
	std::vector<Citizen> citizens;
	RoadNetwork& roadNetwork;
	bool enabled;

	std::vector<VehiclePath> vehiclePaths;



	bool onSameEdge(const Citizen& citizen, const Citizen& otherCitizen) const;
	bool isTooCloseAheadOnTheSameEdge(const Citizen& citizen, const Citizen& otherCitizen, float edgeDistance) const;
	bool isInsideJunction(const Citizen& citizen, const CitizenLayoutContext& layoutContext) const;
	std::pair<CitizenState, bool> getNewValues(Citizen& citizen, const CitizenLayoutContext& layoutContext, VehiclePath* vehPath, float totalTime, float dt) const;

	std::pair<bool, bool> getIndicators(const Citizen& citizen, float newS, float time, const CitizenSimulation::CitizenLayoutContext& layoutContext) const;
	std::pair<bool, bool> getMovementChecks(const Citizen& citizen, const CitizenLayoutContext& layoutContext) const;

	static constexpr float ALLOWED_DISTANCE{ 50.f };

public:

	CitizenSimulation(RoadNetwork& roadNetwork)
		: roadNetwork(roadNetwork), enabled(false) {
	}

	const std::vector<Citizen>& getCitizens() const { return citizens; }
	CitizenLayoutContext getCitizenLayoutContext(const Citizen& citizen) const;
	// NOTE: should simulation rely on getting data directly from graph?
	sf::Vector2f getVertexPos(RoadVertexDescriptor vertex) const { return roadNetwork.getGraph()[vertex].position; }
	std::pair<sf::Vector2f, sf::Vector2f> getCitizenDirection(int citizenId) const;
	std::pair<sf::Vector2f, sf::Vector2f> getCitizenPathDirection(int citizenId, bool next = false) const;
	bool isCitizenOnFirstOrLastEdge(int citizenId) const;

	void enableTest();

	void update(float totalTime, float dt);
	void realUpdate(float totalTime, float dt); // scaffolding



	// vehicle path management
	VehiclePath* getVehiclePath(int id) {

		if (id < 0 || id >= vehiclePaths.size())
			throw std::out_of_range("Vehicle path ID is out of range");

		return &vehiclePaths[id];
	}

	const VehiclePath* getVehiclePath(int id) const {

		if (id < 0 || id >= vehiclePaths.size())
			throw std::out_of_range("Vehicle path ID is out of range");

		return &vehiclePaths[id];
	}
};


