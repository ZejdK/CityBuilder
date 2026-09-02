


#pragma once
#include <SFML/System/Vector2.hpp>
#include "RoadNetwork.hpp"
#include "PlacementState.hpp"
#include "UIMode.hpp"
#include <optional>
#include <string>
#include "RoadSegmentGeometry.hpp"
#include "RoadJunctionGeometry.hpp"



class EditorUI {

	RoadNetwork &roadNetwork;

	const RoadJunctionGeometry *hoveredJunction = nullptr;
	const RoadJunctionGeometry *selectedJunction = nullptr;
	const RoadSegmentGeometry *hoveredRoad = nullptr;

	UIMode uiMode = UIMode::AddRoad; // default mode should be View

	// UIMode::AddRoad
	PlacementState roadPlacementState;
	// selectedPos belongs to UIMode::AddRoad
	// it exists if there is no selectedJunction, to select a new position to add to the road network
	std::optional<sf::Vector2f> selectedPos;

public:

	EditorUI(RoadNetwork &roadNetwork);
	
	void setMode(UIMode newMode);
	void cycleMode();

	void setHoveredRoadElement(const RoadJunctionGeometry *junction, const RoadSegmentGeometry *road);
	const RoadJunctionGeometry *getHoveredJunction() const { return hoveredJunction; }
	const RoadSegmentGeometry *getHoveredRoad() const { return hoveredRoad; }
	
	void selectJunctionOrPos(sf::Vector2f cursorPos);
	void deselectRoadElement();
	std::optional<sf::Vector2f> getSelectedPos() const;
	const RoadJunctionGeometry* getSelectedJunction() const { return selectedJunction; }

	UIMode getMode() const { return uiMode; }
	PlacementState getPlacementState() const { return roadPlacementState; }

	std::string getRoadInfoDisplay() const;
};


