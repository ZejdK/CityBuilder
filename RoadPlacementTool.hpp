


#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "SFML/Graphics/Vertex.hpp"
#include "RoadNetwork.hpp"
#include "RoadTypes.hpp"
#include "Math.hpp"
#include "PlacementState.hpp"



class RoadPlacementTool {

	sf::Font font;
	sf::Text currentModeText;
	sf::CircleShape pointer;
	sf::CircleShape intesersectPointer;
	sf::CircleShape snapPointer;

	sf::Vector2f cursorPos;
	std::optional<sf::Vector2f> snapPosition;
	std::optional<sf::Vector2f> selectedOriginPos;
	std::optional<sf::Vector2f> intersectionPos;

	sf::CircleShape selectedRoadEdgeMarker;

	static constexpr float SNAP_RADIUS { 40.0f };
	const RoadNetwork& roadNetwork;
	PlacementState placementState = PlacementState::Idle;

	void drawIdleStage(sf::RenderWindow& window);
	void drawPlaceStage(sf::RenderWindow& window);

	struct IntersectionResult {
		sf::Vector2f point;
		bool snapped;
	};

	std::optional<sf::Vector2f> GetClosestSnapPoint();
	std::optional<IntersectionResult> GetIntersection();

public:
	RoadPlacementTool(const RoadNetwork &roadNetwork);

	bool selected() const;
	void selectOriginPos();
	void deselectNode();

	RoadElement commitRoad();
	
	void setCursorPos(const sf::Vector2f& pos);

	void draw(sf::RenderWindow& window);
};


