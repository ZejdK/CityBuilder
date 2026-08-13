


#pragma once
#include <SFML/System/Vector2.hpp>
#include "RoadNetwork.hpp"
#include "PlacementState.hpp"
#include "Config.hpp"
#include "UIMode.hpp"



class EditorUI {

	RoadNetwork& roadNetwork;

	std::optional<RoadVertexDescriptor> hoveredVertex;
	std::optional<RoadVertexDescriptor> selectedVertex;

	UIMode uiMode = UIMode::AddRoad; // default mode should be View

	// UIMode::AddRoad
	PlacementState roadPlacementState;
	// selectedPos belongs to UIMode::AddRoad
	// it exists if there is no selectedVertex, to select a new position to add to the road network
	std::optional<sf::Vector2f> selectedPos;

public:

	EditorUI(RoadNetwork &roadNetwork);
	
	void setMode(UIMode newMode);

	void setHoveredJunction(std::optional<RoadVertexDescriptor> junction);
	std::optional<RoadVertexDescriptor> getHoveredJunction() const;
	std::optional<sf::Vector2f> getHoveredJunctionPos() const;
	
	void selectJunctionOrPos(sf::Vector2f cursorPos);
	void deselectJunction();
	std::optional<sf::Vector2f> getSelectedJunctionPos() const;
	std::optional<sf::Vector2f> getSelectedPos() const;



	UIMode getMode() const { return uiMode; }
	PlacementState getPlacementState() const { return roadPlacementState; }



	struct IntersectionResult {
		sf::Vector2f point;
		bool snapped;
	};

	std::optional<EditorUI::IntersectionResult> getRoadIntersection(sf::Vector2f cursorPos, const ConfigGlobal& config) const;
};


