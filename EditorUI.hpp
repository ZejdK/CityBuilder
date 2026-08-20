


#pragma once
#include <SFML/System/Vector2.hpp>
#include "RoadNetwork.hpp"
#include "PlacementState.hpp"
#include "Config.hpp"
#include "UIMode.hpp"
#include <optional>
#include <string>
#include "RoadGraphTypes.hpp"
#include "RoadSegmentGeometry.hpp"



// EditorUI returns RoadJunctionGeometry and RoadSegmentGeometry
//		This one is layer between the road network and the UI renderer
//		RoadNetwork  ->  CityBuilder  ->  EditorUI  ->  UIRenderer



class EditorUI {

	RoadNetwork& roadNetwork;

	std::optional<RoadVertexDescriptor> hoveredVertex;
	std::optional<RoadVertexDescriptor> selectedVertex;
	std::optional<EdgeVertexDescriptor> hoveredEdge;

	UIMode uiMode = UIMode::AddRoad; // default mode should be View

	// UIMode::AddRoad
	PlacementState roadPlacementState;
	// selectedPos belongs to UIMode::AddRoad
	// it exists if there is no selectedVertex, to select a new position to add to the road network
	std::optional<sf::Vector2f> selectedPos;

public:

	EditorUI(RoadNetwork &roadNetwork);
	
	void setMode(UIMode newMode);
	void cycleMode();

	void setHoveredRoadElement(std::optional<RoadVertexDescriptor> junction, std::optional<EdgeVertexDescriptor> road);
	std::optional<RoadVertexDescriptor> getHoveredJunction() const;
	std::optional<sf::Vector2f> getHoveredJunctionPos() const;
	std::optional<RoadSegmentGeometry> getHoveredRoad() const;
	
	void selectJunctionOrPos(sf::Vector2f cursorPos);
	void deselectRoadElement();
	std::optional<sf::Vector2f> getSelectedJunctionPos() const;
	std::optional<sf::Vector2f> getSelectedPos() const;

	std::string getRoadInformation() const;


	UIMode getMode() const { return uiMode; }
	PlacementState getPlacementState() const { return roadPlacementState; }



	struct IntersectionResult {
		sf::Vector2f point;
		bool snapped;
	};

	std::optional<EditorUI::IntersectionResult> getRoadIntersection(sf::Vector2f cursorPos, const ConfigGlobal& config) const;
};


