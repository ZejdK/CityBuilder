


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
#include "VehicleSourceSink.hpp"



struct VehicleSourceSinkPopupState {

	bool requestOpen = false;

	std::optional<int> junctionId = std::nullopt;
	
	std::string label = "Placeholder label";
	float period = 1.0f;
	int option = 4;
	bool active = true;
};



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

	VehicleSourceSinkPopupState vehicleSourceSinkPopup;

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



	VehicleSourceSinkPopupState &getVehicleSourceSinkPopupState() {
		
		return vehicleSourceSinkPopup;
	}

	void requestVehicleSourceSinkPopup(const RoadJunctionGeometry* junction, const VehicleSourceSink* vss) {

		if (hoveredJunction == nullptr || vss == nullptr)
			return;

		vehicleSourceSinkPopup.requestOpen = true;
		vehicleSourceSinkPopup.junctionId = junction->getId();
		vehicleSourceSinkPopup.label = vss->getLabel();
		vehicleSourceSinkPopup.period = vss->getPeriod();
		vehicleSourceSinkPopup.active = vss->isActive();
		vehicleSourceSinkPopup.option = vss->getVehicleColourInt();
	}

	void cancelVehicleSourceSinkPopup() {
		
		vehicleSourceSinkPopup.requestOpen = false;
		vehicleSourceSinkPopup.junctionId = std::nullopt;
	}

	void applyVehicleSourceSinkSettings() {

		if (!vehicleSourceSinkPopup.junctionId)
			throw "No junction id selected!";

		citizenSimulation.configureVehicleSourceSink(*vehicleSourceSinkPopup.junctionId, vehicleSourceSinkPopup.label, vehicleSourceSinkPopup.period, vehicleSourceSinkPopup.active, vehicleSourceSinkPopup.option);
	}
};


