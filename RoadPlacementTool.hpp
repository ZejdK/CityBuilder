


#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "RoadTypes.hpp"



class RoadPlacementTool {

	sf::Font font;
	sf::Text currentModeText;
	sf::CircleShape pointer;
	std::optional<sf::Vector2f> selectedNode;

public:
	RoadPlacementTool();

	bool selected() const;
	void selectNode(const sf::Vector2f &n);
	void deselectNode();

	RoadElement commitRoad(const sf::Vector2f &n);
	
	void setHoverPointer(const sf::Vector2f &pos);

	void draw(sf::RenderWindow& window) const;
};


