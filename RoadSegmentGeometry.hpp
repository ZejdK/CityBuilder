


#pragma once
#include <SFML/System/Vector2.hpp>
#include <array>
#include <optional>
#include "RoadGraphTypes.hpp"
#include <vector>



// This class is geometric representation of a road segment that connects two junctions
//		two junctions may be connected with lanes going one way or the other and because of that
//		this class is in a way a collection of edges and their resulting geometry
// NOTE: endpoints do not have an order
class RoadSegmentGeometry {

	int id;

	RoadVertexDescriptor vertexA;
	RoadVertexDescriptor vertexB;

	std::vector<RoadEdgeDescriptor> edges;
	
	sf::Vector2f start;
	sf::Vector2f end;



	std::array<sf::Vector2f, 4> getVerticesInternal(sf::Vector2f start, sf::Vector2f end, float roadWidth) const;
	
public:

	RoadSegmentGeometry(int id, RoadVertexDescriptor startVertex, RoadVertexDescriptor endVertex, sf::Vector2f start, sf::Vector2f end);

	

	std::array<sf::Vector2f, 4> getVertices(float roadWidth, sf::Vector2f origin) const;

	std::array<std::optional<sf::Vector2f>, 4> getSideIntersectionPoints(float roadWidth, const RoadSegmentGeometry& other, sf::Vector2f side) const;
	std::optional<sf::Vector2f> getClosestSideIntersection(float roadWidth, const RoadSegmentGeometry& other, sf::Vector2f roadJunctionPoint) const;
	std::optional<sf::Vector2f> getFurthestSideIntersection(float roadWidth, const RoadSegmentGeometry& other, sf::Vector2f roadJunctionPoint) const;

	sf::Vector2f getPointOnShoulder(float roadWidth, float distance, bool leftSide, sf::Vector2f endpoint) const;
	sf::Vector2f getProjectedPoint(const sf::Vector2f& point) const;
	sf::Vector2f getMirroredPoint(sf::Vector2f point) const;

	bool isPointOnRoad(float roadWidth, const sf::Vector2f& point) const;

	float length() const { return (end - start).length(); }



	void addEdge(RoadEdgeDescriptor edge) { edges.push_back(edge); }
	std::vector<RoadEdgeDescriptor> getEdges() const { return edges; }

	RoadVertexDescriptor getVertexA() const { return vertexA; }
	RoadVertexDescriptor getVertexB() const { return vertexB; }
	
	sf::Vector2f getStart() const { return start; }
	sf::Vector2f getEnd() const { return end; }

	

	int getId() const { return id; }
};


