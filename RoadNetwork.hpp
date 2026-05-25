


#pragma once
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include "RoadTypes.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "RoadGraphTypes.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>



// class RoadNetwork // TODO: turn this into a class again
struct RoadNetwork {

	RoadGraph roadGraph;
	std::vector<RoadElement> roads{};

	sf::CircleShape graphNodeShape;
	sf::Font font;
	sf::Text text;

	// TODO: clean up added extra nodes, see line below
	//			if i connect an edge to "middle of another edge". i would need to split edge into two edges to create a node to connect to the newly added edge. If i remove this newly added edge, I would then be left with a "trivial" node, ie node with only two edges and is also in the middle of a geometric line. periodically go through all the nodes to clean these up

// public:
	
	RoadNetwork();

	void add(const RoadElement& road);
	void draw(sf::RenderWindow& window) const;
	void drawGraph(sf::RenderWindow& window);
};


