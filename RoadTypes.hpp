


#pragma once
#include <SFML/System/Vector2.hpp>
#include <array>
#include "Math.hpp"



struct RoadElement {

	sf::Vector2f start;
	sf::Vector2f end;

	std::array<sf::Vector2f, 4> getVertices(float roadWidth) const {

		sf::Vector2f direction { (end - start).normalized() };
		sf::Vector2f normal { -direction.y, direction.x }; // rotation matrix for pi/2

		sf::Vector2f offset { normal * roadWidth };

		const float len{ (end - start).length() };

		return {

			start + offset,
			end + offset,
			end - offset,
			start - offset
		};
	}

	float length() const {

		return std::sqrt((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));
	}
};


