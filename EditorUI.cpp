


#include "EditorUI.hpp"
#include <iostream>
#include "Math.hpp"
#include "Config.hpp"
#include <optional>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/detail/adjacency_list.hpp"
#include "SFML/System/Vector2.hpp"
#include "PlacementState.hpp"
#include "RoadGraphTypes.hpp"
#include "RoadNetwork.hpp"
#include "UIMode.hpp"
#include <string>



EditorUI::EditorUI(RoadNetwork& roadNetwork)
    : roadNetwork(roadNetwork), uiMode(UIMode::AddRoad), roadPlacementState(PlacementState::Idle) {}



void EditorUI::setHoveredJunction(std::optional<RoadVertexDescriptor> junction) {

    hoveredVertex = junction;
}

void EditorUI::selectJunctionOrPos(sf::Vector2f cursorPos) {
    
    switch (uiMode) {
    case UIMode::AddRoad:

        if (roadPlacementState == PlacementState::Idle) {

            if (hoveredVertex)
                selectedVertex = hoveredVertex;
            else
                selectedPos = cursorPos;
            
            roadPlacementState = PlacementState::Placing;
        }
        else {
            
            if (selectedPos) {

                if (hoveredVertex)
                    roadNetwork.add(*selectedPos, *hoveredVertex);
                else
                    roadNetwork.add(*selectedPos, cursorPos);
            }
            else if (selectedVertex) {

                if (hoveredVertex)
                    roadNetwork.add(*selectedVertex, *hoveredVertex);
                else
                    roadNetwork.add(*selectedVertex, cursorPos);
            }

            deselectJunction();
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

void EditorUI::deselectJunction() {

    selectedVertex.reset();
    selectedPos.reset();
}

// assumes EditorUI will be in UIMode::AddRoad and PlacementState::Placing
std::optional<sf::Vector2f> EditorUI::getSelectedJunctionPos() const {

    if (!selectedVertex)
        return std::nullopt;

    auto& G = roadNetwork.getGraph();

    return G[*selectedVertex].position;
}

std::optional<sf::Vector2f> EditorUI::getSelectedPos() const {

    if (!selectedVertex && !selectedPos)
        return std::nullopt;

    if (selectedVertex)
        return getSelectedJunctionPos();

    return *selectedPos;
}

std::string EditorUI::getRoadInformation() const {
    
    return roadNetwork.getInformation();
}

std::optional<RoadVertexDescriptor> EditorUI::getHoveredJunction() const {

    return hoveredVertex;
}

std::optional<sf::Vector2f> EditorUI::getHoveredJunctionPos() const {
    
    if (!hoveredVertex)
        return std::nullopt;

    auto &G = roadNetwork.getGraph();

    return G[*hoveredVertex].position;
}


void EditorUI::setMode(UIMode newMode) {

    std::cout << "Setting mode from " << int(uiMode) << " to " << int(newMode) << "\n";

    // perform cleanup from the previous mode
    switch (uiMode) {
    case UIMode::AddRoad:

        roadPlacementState = PlacementState::Idle;
        deselectJunction();
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



// where should this function live?
//  - it doesn't belong in the road network, because it doesn't have anything to do with graph structure
//  - it's purely geometric data, but it's not one segment or one junction
// 
//  - it needs to loop through all the edges, but also do some geometry calculations
//      - solution could be to make another class, like a geometric representation of the whole network
//      - or to make a RoadNetwork getter for all the edges/roads
//    currently this can stay here, but in the future it should be moved elsewhere, perhaps split across RoadNetwork and UIRenderer
// 
// currently it mixes in cursorPos which is not part of editor UI state, it should be part of UIRenderer
//
// this function quietly assumes that the editor is in AddRoad mode and PlacementState is Placing
// currently it's only called in renderAddRoadPlaceStage
std::optional<EditorUI::IntersectionResult> EditorUI::getRoadIntersection(sf::Vector2f cursorPos, const ConfigGlobal& config) const {

    std::optional<EditorUI::IntersectionResult> closestResult;

    auto &G = roadNetwork.getGraph();
    auto [ begin, end ] = boost::edges(G);

    for (auto it { begin }; it != end; ++it) {

        EdgeVertexDescriptor edge = *it;

        auto sourceVertex = boost::source(edge, G);
        auto targetVertex = boost::target(edge, G);

        auto intersection = CB::Math::intersect(G[sourceVertex].position, G[targetVertex].position, selectedVertex.has_value() ? G[*selectedVertex].position : *selectedPos, cursorPos);

        if (intersection) {

            float dist = CB::Math::distance(cursorPos, *intersection);
            if (!closestResult) {

                closestResult = IntersectionResult { *intersection, dist < config.snapRadius };
            }
            else {

                float distClosest = CB::Math::distance(cursorPos, closestResult->point);
                if (dist < distClosest)
                    closestResult = IntersectionResult { *intersection, dist < config.snapRadius };
            }
        }
    }

    return closestResult;
}


