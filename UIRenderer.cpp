


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
#include "RoadNetworkLayout.hpp"



UIRenderer::UIRenderer(ConfigGlobal config)
	: config(config), logger(10.f, 10.f, 30.f), pointer(10.0f), intesersectPointer(10.0f), snapPointer(config.snapRadius),
    roadSelector(sf::PrimitiveType::Triangles, 6), junctionSelector(config.roadWidth) {

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

    // shows the hovered junction in View UI mode
	junctionSelector.setFillColor(sf::Color(0, 255, 0, 100));
	junctionSelector.setOrigin(sf::Vector2f(config.roadWidth, config.roadWidth));
}

void UIRenderer::setCursorPos(const sf::Vector2f& pos) {

    cursorPos = pos;
}



void UIRenderer::render(sf::RenderWindow& window, const EditorUI& editorUi, const RoadNetworkLayout &roadLayout) {
    
    logger.add(std::string("Roads information: ") + editorUi.getRoadInfoDisplay());
    logger.add(std::string("UI mode: ") + std::string(to_string(editorUi.getMode())) + " (Press Q)");

    switch (editorUi.getMode()) {
	case UIMode::AddRoad:
		renderAddRoad(window, editorUi, roadLayout);
		break;
	case UIMode::Select:
        renderRoadSelector(window, editorUi);
		break;
	case UIMode::AddLocation:
		// render add location UI
		break;
	case UIMode::View:
		// render view UI
		break;
    }

    logger.render(window);
    logger.clear();
}



void UIRenderer::renderAddRoad(sf::RenderWindow& window, const EditorUI& editorUi, const RoadNetworkLayout &roadLayout) {

    logger.add(std::format("Add road - Placement state: {}", int(editorUi.getPlacementState())));

    if (editorUi.getPlacementState() == PlacementState::Idle)
        renderAddRoadIdleStage(window, editorUi);
    else
        renderAddRoadPlaceStage(window, editorUi, roadLayout);
}

void UIRenderer::renderAddRoadIdleStage(sf::RenderWindow& window, const EditorUI& editorUi)
{
    auto hoveredJunction = editorUi.getHoveredJunction();

    if (!hoveredJunction) {

        logger.add("add road snap pos: none");
        pointer.setPosition(cursorPos);
        window.draw(pointer);
    }
    else {

        auto snapPosition { hoveredJunction->getPosition() };

        logger.add("add road snap pos: " + std::to_string(snapPosition.x) + std::string(", ") + std::to_string(snapPosition.y));
        snapPointer.setPosition(snapPosition);
        window.draw(snapPointer);
    }
}

// this function assumes that selectedPos actually exists, otherwise it wouldn't be in this state
void UIRenderer::renderAddRoadPlaceStage(sf::RenderWindow& window, const EditorUI& editorUi, const RoadNetworkLayout &roadLayout) {
    
    std::optional<sf::Vector2f> snapPosition{ std::nullopt };
    auto intersectionResult { roadLayout.doesLineIntersectAnyRoad(*editorUi.getSelectedPos(), cursorPos, config.snapRadius)};

    if (editorUi.getHoveredJunction() != nullptr)
        snapPosition = editorUi.getHoveredJunction()->getPosition();

    if (!snapPosition && intersectionResult.exists && intersectionResult.snapped)
        snapPosition = intersectionResult.point;

    if (snapPosition) {

        logger.add("Place state - Roads snapped: OK");
        snapPointer.setPosition(*snapPosition);
        window.draw(snapPointer);
    }

    if (intersectionResult.exists && !intersectionResult.snapped) {

        if (CB::Math::equals(intersectionResult.point, *editorUi.getSelectedPos())) {

            logger.add("Place state - Equal points"); // they snap at the same points in their ends
        }
        else {

            logger.add("Place state - Error: roads intersecting");
            snapPosition = intersectionResult.point;
            intesersectPointer.setPosition(intersectionResult.point);
            window.draw(intesersectPointer);
        }
    }


    
    sf::Vertex v1 { sf::Vector2f { *editorUi.getSelectedPos() } };
    sf::Vertex v2 { sf::Vector2f { snapPosition ? *snapPosition : cursorPos } };
    std::array temp = { v1, v2 };
    window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
}



void UIRenderer::renderRoadSelector(sf::RenderWindow& window, const EditorUI& editorUi) {

    auto roadJunction { editorUi.getHoveredJunction() };
    auto roadSegment { editorUi.getHoveredRoad() };
    
	if (roadJunction) {

		auto junctionPos { roadJunction->getPosition() };
		junctionSelector.setPosition(junctionPos);
		window.draw(junctionSelector);

		logger.add("Road selector - Hovered junction: " + std::to_string(junctionPos.x) + std::string(", ") + std::to_string(junctionPos.y));
	}
    else if (roadSegment) {

        auto vertices = roadSegment->getVertices(config.roadWidth, roadSegment->getStart());

        roadSelector[0].position = vertices[0];
        roadSelector[1].position = vertices[1];
        roadSelector[2].position = vertices[2];
        roadSelector[3].position = vertices[0];
        roadSelector[4].position = vertices[2];
        roadSelector[5].position = vertices[3];

		for (int i { 0 }; i < 6; ++i)
			roadSelector[i].color = sf::Color(0, 255, 0, 100);

        window.draw(roadSelector);

		logger.add("Road selector - Hovered road segment ");
    }
}


