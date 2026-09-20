


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
#include <optional>
#include "CitizenJunctionCurve.hpp"
#include "Config.hpp"
#include <tuple>
#include "VehicleSourceSink.hpp"
#include <string>



class CitizenSimulation
{
	std::vector<Citizen> citizens;
	RoadNetwork& roadNetwork;
	bool enabled;
	ConfigGlobal config;
	float totalTime = 0.f; // totalTime from main loop, cached in here

	std::vector<VehiclePath> vehiclePaths;
	std::vector<VehicleSourceSink> vehicleSourceSinks;



	bool onSameEdge(const Citizen& citizen, const Citizen& otherCitizen) const;
	bool isTooCloseAheadOnTheSameEdge(const Citizen& citizen, const Citizen& otherCitizen, float edgeDistance) const;
	bool isInsideJunction(const Citizen& citizen, const CitizenLayoutContext& layoutContext) const;
	std::pair<CitizenState, bool> getNewValues(Citizen& citizen, const CitizenLayoutContext& layoutContext, VehiclePath* vehPath, float totalTime, float dt) const;

	std::pair<bool, bool> getIndicators(const Citizen& citizen, float newS, float time, const CitizenLayoutContext& layoutContext) const;
	std::pair<bool, bool> getMovementChecks(const Citizen& citizen, const CitizenLayoutContext& layoutContext) const;

	static constexpr float ALLOWED_DISTANCE{ 50.f };

public:

	CitizenSimulation(RoadNetwork& roadNetwork, ConfigGlobal config)
		: roadNetwork(roadNetwork), config(config), enabled(false) {
	}

	const std::vector<Citizen>& getCitizens() const { return citizens; }
	CitizenLayoutContext getCitizenLayoutContext(const Citizen& citizen) const;
	// NOTE: should simulation rely on getting data directly from graph?
	sf::Vector2f getVertexPos(RoadVertexDescriptor vertex) const { return roadNetwork.getGraph()[vertex].position; }
	std::pair<sf::Vector2f, sf::Vector2f> getCitizenDirection(const Citizen& citizen) const;
	std::pair<sf::Vector2f, sf::Vector2f> getCitizenPathDirection(const Citizen& citizen, bool next = false) const;
	std::tuple<sf::Vector2f, sf::Vector2f, std::optional<CitizenJunctionCurve>> getNewPositionAndDirection(const Citizen& citizen) const;
	bool isCitizenOnFirstOrLastEdge(const Citizen& citizen) const;
	bool isCitizenOnLastEdge(const Citizen& citizen) const;
	bool isCitizenStartingPath(const Citizen& citizen) const;
	bool isCitizenFinishingPath(const Citizen& citizen) const;

	void update(float totalTime, float dt);
	void updateCitizens(float totalTime, float dt);
	void updateVehicleSourceSinks(float totalTime, float dt);

	Citizen& createNewCitizen(const std::string& name, const std::string& surname, const std::string& colour);
	const Citizen* findCitizen(sf::Vector2f position) const;



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

	const VehiclePath* copyVehiclePath(int id) {

		if (id < 0 || id >= vehiclePaths.size())
			throw std::out_of_range("Vehicle path ID is out of range");
		
		auto newId{ int(vehiclePaths.size()) };
		auto vehPath{ vehiclePaths[id] };

		vehiclePaths.push_back(VehiclePath{ newId, vehPath.getPath() });
		VehiclePath& newVehPath{ vehiclePaths.back() };
		
		return &newVehPath;
	}

	// Vehicle source sinks
	const std::vector<VehicleSourceSink> &getVehicleSourceSinks() const { return vehicleSourceSinks; }
	void addVehicleSourceSinkShortest(const RoadJunctionGeometry* startJunction, const RoadJunctionGeometry* endJunction, std::string colour) {

		auto shortestPath{ roadNetwork.getShortestPath(startJunction->getVertex(), endJunction->getVertex()) };

		int newVehPathId{ int(vehiclePaths.size()) };
		vehiclePaths.push_back(VehiclePath{ newVehPathId, shortestPath });
		VehiclePath& vehPath{ vehiclePaths.back() };

		constexpr float DEFAULT_PERIOD{ 1.f }; // default period is in seconds
		
		vehicleSourceSinks.push_back(VehicleSourceSink{ std::string("placeholder label"), DEFAULT_PERIOD, newVehPathId, colour, startJunction->getPosition(), endJunction->getPosition() });
		vehicleSourceSinks.back().setActive(true, totalTime);
	}
};


