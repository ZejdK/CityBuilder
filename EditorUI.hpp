


#pragma once
#include <SFML/System/Vector2.hpp>
#include "RoadNetwork.hpp"
#include "UIMode.hpp"
#include <optional>
#include <string>
#include "RoadSegmentGeometry.hpp"
#include "RoadJunctionGeometry.hpp"
#include "Citizen.hpp"
#include "UIStates.hpp"
#include "CitizenSimulation.hpp"
#include <vector>



class EditorUI {

	RoadNetwork &roadNetwork;
	CitizenSimulation& citizenSimulation;

	const RoadJunctionGeometry *hoveredJunction = nullptr;
	const RoadJunctionGeometry *selectedJunction = nullptr;
	const RoadSegmentGeometry *hoveredRoad = nullptr;
	const Citizen *hoveredVehicle = nullptr;
	const Citizen *selectedVehicle = nullptr;

	UIMode uiMode = UIMode::AddRoad; // default mode should be View

	// UIMode::AddRoad
	PlacementState roadPlacementState;
	// selectedPos belongs to UIMode::AddRoad
	// it exists if there is no selectedJunction, to select a new position to add to the road network
	std::optional<sf::Vector2f> selectedPos;


	// UIMode::AddLocation
	AddLocationType addLocationType = AddLocationType::SourceSinkShortestPath;
	AddLocationPlacementState addLocationPlacementState = AddLocationPlacementState::PlacingSource;
	int vehicleSinkSourceColourCounter = 0;
	std::vector<std::string> vehicleSinkSourceColours = { "b", "g", "o", "r", "w" };

public:

	EditorUI(RoadNetwork &roadNetwork, CitizenSimulation& citizenSimulation);
	
	void setMode(UIMode newMode);
	void cycleMode();

	void setHoveredRoadElement(const RoadJunctionGeometry *junction, const RoadSegmentGeometry *road);
	const RoadJunctionGeometry *getHoveredJunction() const { return hoveredJunction; }
	const RoadSegmentGeometry *getHoveredRoad() const { return hoveredRoad; }
	
	void selectJunctionOrPos(sf::Vector2f cursorPos);
	void deselectRoadElement();
	std::optional<sf::Vector2f> getSelectedPos() const;
	const RoadJunctionGeometry* getSelectedJunction() const { return selectedJunction; }

	void setHoveredVehicle(const Citizen* citizen) { hoveredVehicle = citizen; }
	const Citizen *getHoveredVehicle() const { return hoveredVehicle; }

	UIMode getMode() const { return uiMode; }
	PlacementState getPlacementState() const { return roadPlacementState; }

	AddLocationType getAddLocationType() const { return addLocationType; }
	AddLocationPlacementState getAddLocationPlacementState() const { return addLocationPlacementState; }

	std::string getRoadInfoDisplay() const;
};


