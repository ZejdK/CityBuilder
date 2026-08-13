


#include "UIRenderer.hpp"
#include "Math.hpp"
#include <array>
#include "Config.hpp"
#include "EditorUI.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/System/Vector2.hpp"
#include <format>
#include <optional>
#include <string>
#include "SFML/Graphics/PrimitiveType.hpp"
#include "PlacementState.hpp"
#include "UIMode.hpp"



UIRenderer::UIRenderer(ConfigGlobal config)
	: config(config), logger(10.f, 10.f, 30.f), pointer(10.0f), intesersectPointer(10.0f), snapPointer(config.snapRadius) {

    // pointer follows the mouse
    pointer.setFillColor(sf::Color::Blue);
    pointer.setOrigin(sf::Vector2f(10.f, 10.f));

    // snapPointer shows within snapping distance of hovered vertex/junction
    const auto snapRadius = config.snapRadius;
    snapPointer.setFillColor(sf::Color::Magenta);
    snapPointer.setPosition(sf::Vector2f(200.f, 200.f));
    snapPointer.setOrigin(sf::Vector2f(snapRadius, snapRadius));

    // shows up if there's a line intersection with an existing road
    intesersectPointer.setFillColor(sf::Color::Red);
    intesersectPointer.setOrigin(sf::Vector2f(10.f, 10.f));
}

void UIRenderer::setCursorPos(const sf::Vector2f& pos) {

    cursorPos = pos;
}



void UIRenderer::render(sf::RenderWindow& window, const EditorUI& editorUi) {
    
    logger.add(std::string("UI mode: ") + std::string(to_string(editorUi.getMode())));
    logger.add(std::format("Placement state: {}", int(editorUi.getPlacementState())));

    if (editorUi.getPlacementState() == PlacementState::Idle)
        renderAddRoadIdleStage(window, editorUi);
    else
        renderAddRoadPlaceStage(window, editorUi);

    logger.render(window);
    logger.clear();
}

void UIRenderer::renderAddRoadIdleStage(sf::RenderWindow& window, const EditorUI& editorUi)
{
    auto snapPosition = editorUi.getHoveredJunctionPos();

    if (!snapPosition) {

        logger.add("snap pos: none");
        pointer.setPosition(cursorPos);
        window.draw(pointer);
    }
    else {

        logger.add("snap pos: " + std::to_string(snapPosition->x) + std::string(", ") + std::to_string(snapPosition->y));
        snapPointer.setPosition(*snapPosition);
        window.draw(snapPointer);
    }
}

void UIRenderer::renderAddRoadPlaceStage(sf::RenderWindow& window, const EditorUI& editorUi) {

    std::optional<EditorUI::IntersectionResult> intersectionResult { editorUi.getRoadIntersection(cursorPos, config) };

    auto selectedPosition = editorUi.getSelectedPos();
    auto snapPosition = editorUi.getHoveredJunctionPos();

    if (!snapPosition && intersectionResult && intersectionResult->snapped)
        snapPosition = intersectionResult->point;

    if (snapPosition) {

        logger.add("Place state - Roads snapped: OK");
        snapPointer.setPosition(*snapPosition);
        window.draw(snapPointer);
    }

    if (intersectionResult && !intersectionResult->snapped) {

        if (CB::Math::equals(intersectionResult->point, *selectedPosition)) {

            logger.add("Place state - Equal points"); // they snap at the same points in their ends
        }
        else {

            logger.add("Place state - Error: roads intersecting");
            snapPosition = intersectionResult->point;
            intesersectPointer.setPosition(intersectionResult->point);
            window.draw(intesersectPointer);
        }
    }

    
    
    sf::Vertex v1 { sf::Vector2f { *selectedPosition } };
    sf::Vertex v2 { sf::Vector2f { snapPosition ? *snapPosition : cursorPos } };
    std::array temp = { v1, v2 };
    window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
}


