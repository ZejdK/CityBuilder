


#include "RoadPlacementTool.hpp"
#include <iostream>



RoadPlacementTool::RoadPlacementTool(const RoadNetwork &roadNetwork)
        : font("assetstemp/arial.ttf"), currentModeText(font), pointer(10.0f), intesersectPointer(10.0f),
            snapPointer(SNAP_RADIUS), roadNetwork(roadNetwork) {

    currentModeText.setPosition(sf::Vector2f(10.f, 10.f));
    currentModeText.setCharacterSize(24);
    currentModeText.setFillColor(sf::Color::Cyan);
    currentModeText.setStyle(sf::Text::Bold | sf::Text::Underlined);

    pointer.setFillColor(sf::Color::Blue); // pointer has become a circle following the mouse
    pointer.setOrigin(sf::Vector2f(10.f, 10.f));
    
    snapPointer.setFillColor(sf::Color::Magenta);
    snapPointer.setPosition(sf::Vector2f(200.f, 200.f));
    snapPointer.setOrigin(sf::Vector2f(SNAP_RADIUS, SNAP_RADIUS));

    intesersectPointer.setFillColor(sf::Color::Red);
    intesersectPointer.setOrigin(sf::Vector2f(10.f, 10.f));
}

bool RoadPlacementTool::selected() const {

    return selectedOriginPos.has_value();
}

void RoadPlacementTool::selectOriginPos() {

    selectedOriginPos = cursorPos;
    if (snapPosition)
        selectedOriginPos = *snapPosition;

    placementState = PlacementState::Placing;
}

RoadElement RoadPlacementTool::commitRoad() {

    sf::Vector2f destination = cursorPos;
    if (snapPosition)
        destination = *snapPosition;

    RoadElement roadSeg{ *selectedOriginPos, destination };

    selectedOriginPos = std::nullopt;
    placementState = PlacementState::Idle;

    return roadSeg;
}

void RoadPlacementTool::deselectNode() {

    placementState = PlacementState::Idle;
    selectedOriginPos.reset();
}

void RoadPlacementTool::setCursorPos(const sf::Vector2f& pos) {

    cursorPos = pos;
    pointer.setPosition(pos);
}

void RoadPlacementTool::draw(sf::RenderWindow& window) {

    currentModeText.setFillColor(sf::Color::Cyan);
    currentModeText.setPosition(sf::Vector2f(10.f, 10.f));
    if (placementState == PlacementState::Idle)
        currentModeText.setString("Editor mode: selecting origin");
    else
        currentModeText.setString("Editor mode: selecting destination");
    window.draw(currentModeText);

    currentModeText.setPosition(sf::Vector2f(10.0f, 40.0f));
    currentModeText.setString(std::string("Road count: ") + std::to_string(roadNetwork.getRoads().size()));
    window.draw(currentModeText);

    if (selectedOriginPos.has_value())
        drawPlaceStage(window);
    else
        drawIdleStage(window);
}

void RoadPlacementTool::drawIdleStage(sf::RenderWindow& window)
{
	snapPosition = roadNetwork.findJunctionPosNear(SNAP_RADIUS, cursorPos);

    if (!snapPosition)
        window.draw(pointer);
    
    currentModeText.setFillColor(sf::Color::Green);
    currentModeText.setPosition(sf::Vector2f(10.0f, 130.0f));
    std::string snapPosString{ "null optional" };
    if (snapPosition)
        snapPosString = "snap pos: " + std::to_string(snapPosition->x) + std::string(", ") + std::to_string(snapPosition->y);
    currentModeText.setString(snapPosString);

    window.draw(currentModeText);
    if (snapPosition) {

        snapPointer.setPosition(*snapPosition);
        window.draw(snapPointer);
    }
}

void RoadPlacementTool::drawPlaceStage(sf::RenderWindow& window)
{
    snapPosition = roadNetwork.findJunctionPosNear(SNAP_RADIUS, cursorPos);

    std::optional<IntersectionResult> intersectionResult { GetIntersection() };

    if (!snapPosition && intersectionResult && intersectionResult->snapped)
        snapPosition = intersectionResult->point;

    if (snapPosition) {

        snapPointer.setPosition(*snapPosition);
        window.draw(snapPointer);

        currentModeText.setFillColor(sf::Color::Green);
        currentModeText.setPosition(sf::Vector2f(10.0f, 70.0f));
        currentModeText.setString("Roads snapped: OK");
        window.draw(currentModeText);
    }

    if (intersectionResult && !intersectionResult->snapped) {

        if (CB::Math::equals(intersectionResult->point, *selectedOriginPos)) {

            currentModeText.setFillColor(sf::Color::Red);
            currentModeText.setPosition(sf::Vector2f(10.0f, 180.0f));
            currentModeText.setString("Equal points");
            window.draw(currentModeText);
        }
        else {

            snapPosition = intersectionResult->point;

            intesersectPointer.setPosition(intersectionResult->point);
            window.draw(intesersectPointer);

            currentModeText.setFillColor(sf::Color::Red);
            currentModeText.setPosition(sf::Vector2f(10.0f, 100.0f));
            currentModeText.setString("Error: roads intersecting");
            window.draw(currentModeText);
        }
    }

    // duplicated code for rendering lines/roads
    sf::Vector2f selectedDestPos { snapPosition ? *snapPosition : cursorPos };
    std::array temp = { sf::Vertex{*selectedOriginPos}, sf::Vertex{selectedDestPos} };
    window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
}

// should I make it clearer that this is line intersection? used to snap roads primarily
// tests the CLOSEST line for intersection, can return a value for intersection, but snapped false
// probably should give it a better name as well
// NOTE: input of this is the cursor position
std::optional<RoadPlacementTool::IntersectionResult> RoadPlacementTool::GetIntersection() {

    std::optional<IntersectionResult> closestResult;

    for (const auto& road : roadNetwork.getRoads()) {

        std::optional<sf::Vector2f> intersection = CB::Math::intersect(road.start, road.end, *selectedOriginPos, cursorPos);
        
        if (intersection) {

            float dist = CB::Math::distance(cursorPos, *intersection);
            if (!closestResult) {

                closestResult = IntersectionResult{ *intersection, dist < SNAP_RADIUS };
            }
            else {
                
                float distClosest = CB::Math::distance(cursorPos, closestResult->point);
                if (dist < distClosest)
                    closestResult = IntersectionResult{ *intersection, dist < SNAP_RADIUS };
            }
        }
    }

    return closestResult;
}


