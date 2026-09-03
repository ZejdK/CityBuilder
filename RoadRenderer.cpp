


#include "RoadRenderer.hpp"
#include <SFML/System/Vector2.hpp>
#include "SFML/Graphics/PrimitiveType.hpp"
#include <array>
#include <string>
#include <vector>
#include "CityView.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/detail/adjacency_list.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "RoadGraphTypes.hpp"
#include "RoadJunctionGeometry.hpp"
#include "RoadSegmentGeometry.hpp"
#include "Config.hpp"
#include "RoadNetworkLayout.hpp"



RoadRenderer::RoadRenderer(const ConfigGlobal &config)
        : config(config), graphNodeShape(15.f), debugShape(10.f), font("assetstemp/arial.ttf"), text(font), vertices(sf::PrimitiveType::Triangles, 6), states() {

    if (!texture.loadFromFile("assets/road_dotted.png"))
        throw "Unable to load the texture";

    if (!texturePlain.loadFromFile("assets/road_plain.png"))
        throw "Unable to load the texture: road_plain.png";

	if (!crossingTexture.loadFromFile("assets/crossing.png"))
		throw "Unable to load the texture: crossing.png";

    texture.setRepeated(true);
    crossingTexture.setRepeated(true);
    states.texture = &texture;

    text.setPosition(sf::Vector2f(10.f, 10.f));
    text.setCharacterSize(24);
    text.setFillColor(sf::Color(220, 170, 180));

    graphNodeShape.setOrigin(sf::Vector2f(15.f, 15.f));
    graphNodeShape.setFillColor(sf::Color(140, 0, 50));

    debugShape.setOrigin(sf::Vector2f(10.f, 10.f));
    debugShape.setFillColor(sf::Color(0, 155, 155, 155));
    
    junctionTriangle.setPointCount(3);
    junctionTriangle.setFillColor(sf::Color(91, 91, 91)); // textures are (71, 71, 71)
}

void RoadRenderer::render(sf::RenderWindow& window, const RoadNetworkLayout &roadLayout, const RoadGraph &roadGraph, CityView cityView) {

    switch (cityView) {

    case CityView::Road:
        renderRoads(window, roadLayout);
        renderJunctions(window, roadLayout);
        break;
    case CityView::Graph:
        renderGraph(window, roadGraph);
        break;
    case CityView::Line:
        // Intentionally left empty: line-only view handled elsewhere if needed
        break;
    case CityView::RoadLineGraph:
        renderRoads(window, roadLayout);
        renderJunctions(window, roadLayout);
        renderGraph(window, roadGraph);
        break;
    }
}



void RoadRenderer::renderGraph(sf::RenderWindow& window, const RoadGraph &roadGraph) {

    auto [ verticesBegin, verticesEnd ] = boost::vertices(roadGraph);
    for (auto it{ verticesBegin }; it != verticesEnd; ++it)
        renderVertex(window, *it, roadGraph);

    auto [ it, end ] = boost::edges(roadGraph);

    for (; it != end; ++it)
        renderEdge(window, *it, roadGraph);
}

void RoadRenderer::renderVertex(sf::RenderWindow& window, RoadVertexDescriptor vertex, const RoadGraph& roadGraph) {

    auto junction = roadGraph[vertex];
    const auto degreeIn = boost::in_degree(vertex, roadGraph);
    const auto degreeOut = boost::out_degree(vertex, roadGraph);

    text.setPosition(junction.position);
    text.setString(std::format("i{}o{}", degreeIn, degreeOut));
    graphNodeShape.setPosition(junction.position);
    window.draw(graphNodeShape);
    window.draw(text);
}

void RoadRenderer::renderEdge(sf::RenderWindow& window, RoadEdgeDescriptor edge, const RoadGraph& roadGraph) {

    auto sourceVertex = boost::source(edge, roadGraph);
    auto targetVertex = boost::target(edge, roadGraph);
    const RoadNodeData& sourceNode = roadGraph[sourceVertex];
    const RoadNodeData& targetNode = roadGraph[targetVertex];

    renderLine(window, sourceNode.position, targetNode.position);

    const RoadEdgeData& edgeData = roadGraph[edge];
    sf::Vector2f middle { (sourceNode.position + targetNode.position) / 2.f };

    text.setPosition(middle);
    text.setString(std::to_string(edgeData.length));
    window.draw(text);
}

void RoadRenderer::renderLine(sf::RenderWindow& window, sf::Vector2f origin, sf::Vector2f destination) {

    std::array temp = { sf::Vertex{origin}, sf::Vertex{destination} };
    window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
}



void RoadRenderer::renderRoads(sf::RenderWindow& window, const RoadNetworkLayout& roadLayout) {

    states.texture = &texture;
    
    for (const auto &roadSegment : roadLayout.getRoads())
        renderRoad(window, roadSegment);
}

void RoadRenderer::renderRoad(sf::RenderWindow& window, const RoadSegmentGeometry &re) {

    auto len = re.length();
    auto v = re.getVertices(config.roadWidth, re.getStart());
    
    vertices[0].position = v[0];
    vertices[1].position = v[1];
    vertices[2].position = v[2];
    vertices[3].position = v[0];
    vertices[4].position = v[2];
    vertices[5].position = v[3];
    vertices[0].texCoords = { 0.f, 0.f };
    vertices[1].texCoords = { 0.f, len };
    vertices[2].texCoords = { 256.f, len };
    vertices[3].texCoords = { 0.f, 0.f };
    vertices[4].texCoords = { 256.f, len };
    vertices[5].texCoords = { 256.f, 0.f };

    window.draw(vertices, states);
}



void RoadRenderer::renderJunctions(sf::RenderWindow& window, const RoadNetworkLayout& roadLayout) {

    states.texture = &texturePlain;

	for (const auto &[ id, junction ] : roadLayout.getJunctions())
        if (junction.getConnectedCount() > 2) {

            renderJunctionCenter(window, junction);
            renderJunctionCrossings(window, junction);
        }
}

void RoadRenderer::renderJunctionCenter(sf::RenderWindow& window, const RoadJunctionGeometry& roadSegmentJunction) {

    const std::vector<sf::Vector2f> &junctionPoints { roadSegmentJunction.getJunctionPoints() };
	
    for (int i{ 0 }; i < junctionPoints.size(); i += 4) {

        junctionTriangle.setPoint(0, roadSegmentJunction.getPosition());
        junctionTriangle.setPoint(1, junctionPoints[i]);
        junctionTriangle.setPoint(2, junctionPoints[i + 1]);
        window.draw(junctionTriangle);

        junctionTriangle.setPoint(1, junctionPoints[i]);
        junctionTriangle.setPoint(2, junctionPoints[(i + 5) % junctionPoints.size()]);
        window.draw(junctionTriangle); // this triangle fills in the gaps between the triangles above
    }
}

void RoadRenderer::renderJunctionCrossings(sf::RenderWindow& window, const RoadJunctionGeometry& roadSegmentJunction) {
    
    states.texture = &crossingTexture;
    const std::vector<sf::Vector2f> &junctionPoints { roadSegmentJunction.getJunctionPoints() };

    for (int i{ 0 }; i < junctionPoints.size(); i += 4) {

        //debugShape.setFillColor(sf::Color(255, 255, 0, 100)); // yellow
        //debugShape.setPosition(junctionPoints[i + 1]); // side intersection point
        //window.draw(debugShape);
        //debugShape.setFillColor(sf::Color(0, 0, 255, 100)); // blue
        //debugShape.setPosition(junctionPoints[i]); // side intersection mirror point
        //window.draw(debugShape);
        //debugShape.setFillColor(sf::Color(255, 0, 0, 100)); // red
        //debugShape.setPosition(junctionPoints[i + 2]); // road crossing point 1
        //window.draw(debugShape);
        //debugShape.setFillColor(sf::Color(0, 255, 0, 100)); // green
        //debugShape.setPosition(junctionPoints[i + 3]); // road crossing point 2
        //window.draw(debugShape);

        sf::Vector2f v0 = junctionPoints[i + 1]; // side intersection point
        sf::Vector2f v1 = junctionPoints[i];     // side intersection mirror point
        sf::Vector2f v2 = junctionPoints[i + 3]; // road crossing point 1
        sf::Vector2f v3 = junctionPoints[i + 2]; // road crossing point 2

        vertices[0].position = v0;
        vertices[1].position = v1;
        vertices[2].position = v2;
        vertices[0].texCoords = { 256.f,   0.f };
        vertices[1].texCoords = { 0.f,     0.f };
        vertices[2].texCoords = { 0.f,   128.f };

        vertices[3].position = v0;
        vertices[4].position = v2;
        vertices[5].position = v3;
        vertices[3].texCoords = { 256.f,   0.f };
        vertices[4].texCoords = { 0.f,   128.f };
        vertices[5].texCoords = { 256.f, 128.f };

        window.draw(vertices, states);
    }

    renderDebugSPoints(window, roadSegmentJunction);
}

void RoadRenderer::renderDebugSPoints(sf::RenderWindow& window, const RoadJunctionGeometry& junction) {

    auto junctionRoads{ junction.getConnectedRoads() };

    for (int i{ 0 }; i < junctionRoads.size(); ++i) {

        debugShape.setFillColor(sf::Color(255, 0, 255, 100)); // magenta
        debugShape.setPosition(junction.getSPos(junctionRoads[i]));
        window.draw(debugShape);
    }
}


