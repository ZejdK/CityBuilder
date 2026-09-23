


#include "imgui.h"
#include "imgui-SFML.h"
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
#include "UIStates.hpp"
#include "UIMode.hpp"
#include "RoadNetworkLayout.hpp"
#include "RoadSegmentGeometry.hpp"
#include "RoadJunctionGeometry.hpp"
#include "Citizen.hpp"
#include "CitizenSimulation.hpp"



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



void UIRenderer::render(sf::RenderWindow& window, const EditorUI& editorUi, const RoadNetworkLayout &roadLayout, const CitizenSimulation& citizenSimulation) {
    
    logger.add(std::string("Roads information: ") + editorUi.getRoadInfoDisplay());
    logger.add(std::string("UI mode: ") + std::string(to_string(editorUi.getMode())) + " (Press Q)");

    switch (editorUi.getMode()) {
	case UIMode::AddRoad:
		renderAddRoad(window, editorUi, roadLayout);
		break;
	case UIMode::Select:
        renderVehicleSelector(window, editorUi);
        renderRoadSelector(window, editorUi);
		break;
	case UIMode::AddLocation:

		renderAddLocation(window, editorUi, roadLayout, citizenSimulation);
		break;
	case UIMode::View:
		// render view UI
		break;
    }

    logger.render(window);
    logger.clear();
}

void UIRenderer::renderRoadElementInfoImgui(const EditorUI& editorUi, ImVec2 defaultPos) const {

    if (editorUi.getMode() != UIMode::Select)
        return;

    ImGui::SetNextWindowPos(cursorPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

    if (auto roadJunction { editorUi.getHoveredJunction() }) {

        ImGui::Begin("Road junction", nullptr, flags);
		
        ImGui::Text("Junction ID: %d | Vertex descriptor: %d", roadJunction->getId(), roadJunction->getVertex());
		ImGui::Text("Position: (%.2f, %.2f)", roadJunction->getPosition().x, roadJunction->getPosition().y);
		
        ImGui::Text("Connected roads: %zu", roadJunction->getConnectedCount()); // %zu is for size_t
        // NOTE: roads are already sorted clockwise during the junction creation
        for (const auto& road : roadJunction->getConnectedRoads())
			renderRoadSegmentInfoImgui(road, roadJunction);

        ImGui::End();
    }
    else if (auto roadSegment { editorUi.getHoveredRoad() }) {

        ImGui::Begin("Road segment");
        renderRoadSegmentInfoImgui(roadSegment, nullptr);
        ImGui::End();
    }
}

void UIRenderer::renderRoadSegmentInfoImgui(const RoadSegmentGeometry* road, const RoadJunctionGeometry* roadJunction) const {

    ImGui::Text("   - ID: %d, len: %.2f, s: %.6f", road->getId(), road->length(), roadJunction == nullptr ? 0.0f : roadJunction->getS(road));
    ImGui::Text("      Vertices: %d, %d", road->getVertexA(), road->getVertexB());
    ImGui::Text("      Pos: (%.1f, %.1f), (%.1f, %.1f)", road->getStart().x, road->getStart().y, road->getEnd().x, road->getEnd().y);
}

bool UIRenderer::renderCitizenInfoImgui(const EditorUI &editorUi) const {

    auto citizen{ editorUi.getHoveredVehicle() };

    if (editorUi.getMode() != UIMode::Select || citizen == nullptr)
        return false;

    ImGui::SetNextWindowPos(citizen->getPosition(), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

    ImGui::Begin("Citizen info", nullptr, flags);
	
    ImGui::Text("Citizen ID: %d, Veh colour: %s", citizen->getId(), citizen->getColour().c_str());
	ImGui::Text("Name: %s", citizen->getFullName().c_str());
	ImGui::Text("------------");

	ImGui::Text("Active: %s", citizen->isActive() ? "true" : "false");
    ImGui::Text("Path id: %d | s: %.6f", citizen->getPathId(), citizen->getS());
    ImGui::Text("Inside jun: %s", citizen->isInsideJunction() ? "true" : "false");
    ImGui::Text("Indicating: L: %s | R: %s", citizen->isIndicatingLeft() ? "1" : "0", citizen->isIndicatingRight() ? "1" : "0");

    ImGui::End();

    return true;
}

void UIRenderer::renderVehicleSourceSinkPopupImgui(EditorUI& editorUi, const CitizenSimulation& citizenSimulation) {

    auto& popup = editorUi.getVehicleSourceSinkPopupState();

    if (popup.requestOpen) {

        ImGui::OpenPopup("vehicle_source_sink");
        popup.requestOpen = false;
    }

    if (!ImGui::BeginPopup("vehicle_source_sink"))
        return;

    constexpr const char* options[] = { "b", "g", "o", "r", "w" };
    ImGui::InputText("Label", popup.label.data(), popup.label.size());
    ImGui::InputFloat("Period", &popup.period);
    ImGui::Combo("Option", &popup.option, options, 5);
    ImGui::Checkbox("Active", &popup.active);
    ImGui::Separator();

    if (ImGui::Button("Apply"))
    {
        editorUi.applyVehicleSourceSinkSettings();
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine(); // places the same widget horizontally on the same line

    if (ImGui::Button("Cancel")) {

        editorUi.cancelVehicleSourceSinkPopup();
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
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



void UIRenderer::renderAddLocation(sf::RenderWindow& window, const EditorUI& editorUi, const RoadNetworkLayout& roadLayout, const CitizenSimulation &citizenSimulation) {

    logger.add("Add location - type, state: " + std::to_string(int(editorUi.getAddLocationType())) + " " + std::to_string(int(editorUi.getAddLocationPlacementState())));
    
	if (editorUi.getAddLocationType() == AddLocationType::SourceSinkShortestPath)
        renderAddLocationSourceSink(window, editorUi, roadLayout, citizenSimulation);
}

void UIRenderer::renderAddLocationSourceSink(sf::RenderWindow& window, const EditorUI& editorUi, const RoadNetworkLayout& roadLayout, const CitizenSimulation& citizenSimulation) {

    if (editorUi.getAddLocationPlacementState() == AddLocationPlacementState::PlacingSource) {
        
        auto hoveredJunction{ editorUi.getHoveredJunction() };
        if (!hoveredJunction) {

            logger.add("add location snap pos: none");
            pointer.setPosition(cursorPos);
            window.draw(pointer);
        }
        else {

            auto snapPosition{ hoveredJunction->getPosition() };
            logger.add("add location snap pos: " + std::to_string(snapPosition.x) + std::string(", ") + std::to_string(snapPosition.y));
            snapPointer.setPosition(snapPosition);
            window.draw(snapPointer);
        }
    }
	else if (editorUi.getAddLocationPlacementState() == AddLocationPlacementState::PlacingSink) {

		auto hoveredJunction{ editorUi.getHoveredJunction() };
		if (!hoveredJunction) {
			logger.add("add location snap pos: none");
			pointer.setPosition(cursorPos);
			window.draw(pointer);
		}
		else {
			auto snapPosition{ hoveredJunction->getPosition() };
			logger.add("add location snap pos: " + std::to_string(snapPosition.x) + std::string(", ") + std::to_string(snapPosition.y));
			snapPointer.setPosition(snapPosition);
			window.draw(snapPointer);
		}
	}
}



void UIRenderer::renderRoadSelector(sf::RenderWindow& window, const EditorUI& editorUi) {

    if (editorUi.getHoveredVehicle())
        return;

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

void UIRenderer::renderVehicleSelector(sf::RenderWindow& window, const EditorUI& editorUi) {

    if (auto hoveredVehicle{ editorUi.getHoveredVehicle() }) {

        auto vehiclePos{ hoveredVehicle->getPosition() };
        auto vehicleDir{ hoveredVehicle->getDirection() };

        auto vertices = getVehicleVertices(vehiclePos, vehicleDir);

        roadSelector[0].position = vertices[0];
        roadSelector[1].position = vertices[1];
        roadSelector[2].position = vertices[2];
        roadSelector[3].position = vertices[0];
        roadSelector[4].position = vertices[2];
        roadSelector[5].position = vertices[3];

        for (int i{ 0 }; i < 6; ++i)
            roadSelector[i].color = sf::Color(0, 255, 0, 100);

        window.draw(roadSelector);

        intesersectPointer.setPosition(vehiclePos);
        window.draw(intesersectPointer);
        logger.add("Vehicle hovered");
    }
    else {

        logger.add("Vehicle not hovered");
    }
}

std::array<sf::Vector2f, 4> UIRenderer::getVehicleVertices(sf::Vector2f pos, sf::Vector2f dir) {

    return {

       pos + CB::Math::rotateInDirection(1.5f * sf::Vector2f(20.f, 10.f), dir),   // top right
       pos + CB::Math::rotateInDirection(1.5f * sf::Vector2f(20.f, -10.f), dir),  // bottom right
       pos + CB::Math::rotateInDirection(1.5f * sf::Vector2f(-20.f, -10.f), dir), // bottom left
       pos + CB::Math::rotateInDirection(1.5f * sf::Vector2f(-20.f, 10.f), dir),  // top left
    };
}


