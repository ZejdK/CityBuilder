


#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "EditorUI.hpp"
#include "MessageLog.hpp"
#include "Config.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "RoadNetworkLayout.hpp"
#include "imgui.h"
#include "RoadJunctionGeometry.hpp"
#include "RoadSegmentGeometry.hpp"
#include <array>
#include "CitizenSimulation.hpp"



class UIRenderer {

	ConfigGlobal config;

	MessageLog logger;

	sf::CircleShape pointer;
	sf::CircleShape intesersectPointer;
	sf::CircleShape snapPointer;

	sf::VertexArray roadSelector;
	sf::CircleShape junctionSelector;

	sf::Vector2f cursorPos;

	void renderAddRoad(sf::RenderWindow& window, const EditorUI& editorUi, const RoadNetworkLayout &roadLayout);
	void renderAddRoadIdleStage(sf::RenderWindow &window, const EditorUI& editorUi);
	void renderAddRoadPlaceStage(sf::RenderWindow &window, const EditorUI& editorUi, const RoadNetworkLayout &roadLayout);

	void renderAddLocation(sf::RenderWindow& window, const EditorUI& editorUi, const RoadNetworkLayout& roadLayout, const CitizenSimulation& citizenSimulation);
	void renderAddLocationSourceSink(sf::RenderWindow& window, const EditorUI& editorUi, const RoadNetworkLayout& roadLayout, const CitizenSimulation& citizenSimulation);
	
	void renderRoadSelector(sf::RenderWindow& window, const EditorUI& editorUi);
	void renderVehicleSelector(sf::RenderWindow& window, const EditorUI& editorUi);
	std::array<sf::Vector2f, 4> getVehicleVertices(sf::Vector2f pos, sf::Vector2f dir);
	
public:

	UIRenderer(ConfigGlobal config);

	void setCursorPos(const sf::Vector2f& pos);

	void render(sf::RenderWindow& window, const EditorUI &editorUi, const RoadNetworkLayout &roadLayout, const CitizenSimulation& citizenSimulation);
	void renderRoadElementInfoImgui(const EditorUI& editorUi, ImVec2 defaultPos) const;
	void renderRoadSegmentInfoImgui(const RoadSegmentGeometry* road, const RoadJunctionGeometry* roadJunction) const;
	bool renderCitizenInfoImgui(const EditorUI &editor) const;
};


