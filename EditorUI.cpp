


#include "EditorUI.hpp"
#include <iostream>
#include <optional>
#include "SFML/System/Vector2.hpp"
#include "PlacementState.hpp"
#include "UIMode.hpp"
#include <string>
#include <format>
#include "RoadSegmentGeometry.hpp"
#include "RoadJunctionGeometry.hpp"



EditorUI::EditorUI(RoadNetwork& roadNetwork)
    : hoveredJunction(nullptr), selectedJunction(nullptr), hoveredRoad(nullptr),
      roadNetwork(roadNetwork), uiMode(UIMode::AddRoad), roadPlacementState(PlacementState::Idle) {}



void EditorUI::setHoveredRoadElement(const RoadJunctionGeometry *junction, const RoadSegmentGeometry *road) {

    hoveredJunction = junction;
    hoveredRoad = road;
}

void EditorUI::selectJunctionOrPos(sf::Vector2f cursorPos) {
    
    switch (uiMode) {
    case UIMode::AddRoad:

        if (roadPlacementState == PlacementState::Idle) {

            if (hoveredJunction)
                selectedJunction = hoveredJunction;
            else
                selectedPos = cursorPos;
            
            roadPlacementState = PlacementState::Placing;
        }
        else {
            
            // should there be some kinda extra layer here, between EditorUI and RoadNetwork
            // probably should make some kinda state manager here, like strategy pattern
            if (selectedPos) {

                if (hoveredJunction)
                    roadNetwork.add(*selectedPos, hoveredJunction->getVertex());
                else
                    roadNetwork.add(*selectedPos, cursorPos);
            }
            else if (selectedJunction) {

                if (hoveredJunction)
                    roadNetwork.add(selectedJunction->getVertex(), hoveredJunction->getVertex());
                else
                    roadNetwork.add(selectedJunction->getVertex(), cursorPos);
            }

            deselectRoadElement();
            roadPlacementState = PlacementState::Idle;
        }
        break;
    case UIMode::Select:
        
        // 
        break;
    case UIMode::View:
        
        // no interactions in this mode
        break;
    }
}

void EditorUI::deselectRoadElement() {

    selectedPos.reset();

    selectedJunction = nullptr;
    hoveredJunction = nullptr;
    hoveredRoad = nullptr;
}

std::optional<sf::Vector2f> EditorUI::getSelectedPos() const {

    if (!selectedJunction && !selectedPos)
        return std::nullopt;

    if (selectedJunction)
        return selectedJunction->getPosition();

    return *selectedPos;
}

std::string EditorUI::getRoadInfoDisplay() const {
    
	auto roadInfo = roadNetwork.getInformation();
	auto layoutInfo = roadNetwork.getLayout().getInformation();
    
	return std::format("vertices:{}, edges:{}, junctions:{}, roads:{}", roadInfo.vertexCount, roadInfo.edgeCount, layoutInfo.junctionCount, layoutInfo.roadCount);
}



void EditorUI::setMode(UIMode newMode) {

    std::cout << "Setting mode from " << int(uiMode) << " to " << int(newMode) << "\n";

    // perform cleanup from the previous mode
    switch (uiMode) {
    case UIMode::AddRoad:

        roadPlacementState = PlacementState::Idle;
        deselectRoadElement();
        break;
    case UIMode::Select:
        //
        break;
    case UIMode::View:
        //
        break;
    }

    uiMode = newMode;
}

void EditorUI::cycleMode() {

    setMode(getNextMode(uiMode));
}


