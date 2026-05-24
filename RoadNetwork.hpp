


#pragma once
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include "RoadTypes.hpp"



// class RoadNetwork // TODO: turn this into a class again
struct RoadNetwork {

	std::vector<RoadElement> roads{};

// public:

	void add(const RoadElement& road);
	void draw(sf::RenderWindow& window) const;
};


