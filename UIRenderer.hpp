


#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "SFML/Graphics/Vertex.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "EditorUI.hpp"
#include "MessageLog.hpp"
#include "Config.hpp"
#include "SFML/Graphics/VertexArray.hpp"



class UIRenderer {

	ConfigGlobal config;

	MessageLog logger;

	sf::CircleShape pointer;
	sf::CircleShape intesersectPointer;
	sf::CircleShape snapPointer;

	sf::VertexArray roadSelector;
	sf::CircleShape junctionSelector;

	sf::Vector2f cursorPos;

	void renderAddRoad(sf::RenderWindow& window, const EditorUI& editorUi);
	void renderAddRoadIdleStage(sf::RenderWindow &window, const EditorUI& editorUi);
	void renderAddRoadPlaceStage(sf::RenderWindow &window, const EditorUI& editorUi);

	void renderRoadSelector(sf::RenderWindow& window, const EditorUI& editorUi);

public:

	UIRenderer(ConfigGlobal config);

	void setCursorPos(const sf::Vector2f& pos);

	void render(sf::RenderWindow& window, const EditorUI &editorUi);
};


