


#include "RoadRenderer.hpp"
#include <SFML/System/Vector2.hpp>
#include "SFML/Graphics/PrimitiveType.hpp"



RoadRenderer::RoadRenderer()
        : graphNodeShape(15.f), debugShape(10.f), font("assetstemp/arial.ttf"), text(font), vertices(sf::PrimitiveType::Triangles, 6), states() {

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

void RoadRenderer::render(sf::RenderWindow& window, const RoadNetwork& roadNetwork, CityView cityView) {

    switch (cityView) {

    case CityView::Road:
        renderRoads(window, roadNetwork);
        renderJunctions(window, roadNetwork);
        break;
    case CityView::Graph:
        renderGraph(window, roadNetwork);
        break;
    case CityView::Line:
        // Intentionally left empty: line-only view handled elsewhere if needed
        break;
    case CityView::RoadLineGraph:
        renderRoads(window, roadNetwork);
        renderJunctions(window, roadNetwork);
        renderGraph(window, roadNetwork);
        break;
    }
}



void RoadRenderer::renderGraph(sf::RenderWindow& window, const RoadNetwork &roadNetwork) {

    auto roadGraph = roadNetwork.getGraph();
    
    auto [ it, end ] = boost::edges(roadGraph);

    for (; it != end; ++it)
        renderEdge(window, *it, roadGraph);
}

void RoadRenderer::renderEdge(sf::RenderWindow& window, EdgeVertexDescriptor edge, const RoadGraph& roadGraph) {

    auto sourceVertex = boost::source(edge, roadGraph);
    auto targetVertex = boost::target(edge, roadGraph);
    const RoadNodeData& sourceNode = roadGraph[sourceVertex];
    const RoadNodeData& targetNode = roadGraph[targetVertex];
    const auto sourceDegree = boost::out_degree(sourceVertex, roadGraph);
    const auto targetDegree = boost::out_degree(targetVertex, roadGraph);

    graphNodeShape.setPosition(sourceNode.position);
    text.setPosition(sourceNode.position);
    text.setString(std::to_string(sourceDegree));
    window.draw(graphNodeShape);
    window.draw(text);

    text.setPosition(targetNode.position);
    text.setString(std::to_string(targetDegree));
    graphNodeShape.setPosition(targetNode.position);
    window.draw(graphNodeShape);
    window.draw(text);

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



void RoadRenderer::renderRoads(sf::RenderWindow& window, const RoadNetwork& roadNetwork) {

    auto roadGraph = roadNetwork.getGraph();
    auto [ it, end ] = boost::edges(roadGraph);

    states.texture = &texture;

    for (; it != end; ++it) {

        auto edge = *it;
        auto sourceVertex = boost::source(edge, roadGraph);
        auto targetVertex = boost::target(edge, roadGraph);

        const RoadSegmentGeometry re { roadGraph[sourceVertex].position, roadGraph[targetVertex].position };
        renderRoad(window, re);
    }
}

void RoadRenderer::renderRoad(sf::RenderWindow& window, const RoadSegmentGeometry &re) {

    auto len = re.length();
    auto v = re.getVertices(ROAD_WIDTH);
    
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



// NOTE: RoadRenderer should not have direct access to the RoadNetwork's internal graph structure
void RoadRenderer::renderJunctions(sf::RenderWindow& window, const RoadNetwork& roadNetwork) {

    auto roadGraph = roadNetwork.getGraph();
    auto [ begin, end ] = boost::vertices(roadGraph);

    states.texture = &texturePlain;
    for (auto it { begin }; it != end; ++it)
        if (boost::out_degree(*it, roadGraph) > 2) {

			auto clockwiseRoads = roadNetwork.getJunctionRoadsClockwise(*it);
			RoadJunctionGeometry junction { roadGraph[*it].position, clockwiseRoads };
            renderJunction(window, junction);
        }
}

void RoadRenderer::renderJunction(sf::RenderWindow& window, const RoadJunctionGeometry& roadSegmentJunction) {

    std::vector<sf::Vector2f> junctionPoints{ roadSegmentJunction.getJunctionPoints(ROAD_WIDTH, ROAD_CROSSING_SEGMENT_LENGTH) };

    states.texture = &crossingTexture;
    renderJunctionCrossings(window, roadSegmentJunction, junctionPoints);
    renderJunctionCenter(window, roadSegmentJunction, junctionPoints);
}

void RoadRenderer::renderJunctionCrossings(sf::RenderWindow& window, const RoadJunctionGeometry& roadSegmentJunction, const std::vector<sf::Vector2f> &junctionPoints) {

    for (int i{ 0 }; i < junctionPoints.size(); i += 4) {

        junctionTriangle.setPoint(0, roadSegmentJunction.getPosition());
        junctionTriangle.setPoint(1, junctionPoints[i]);
        junctionTriangle.setPoint(2, junctionPoints[i + 1]);
        window.draw(junctionTriangle);

        junctionTriangle.setPoint(1, junctionPoints[i]);
        junctionTriangle.setPoint(2, junctionPoints[(i + 5) % junctionPoints.size()]);
        window.draw(junctionTriangle);
    }
}

void RoadRenderer::renderJunctionCenter(sf::RenderWindow& window, const RoadJunctionGeometry& roadSegmentJunction, const std::vector<sf::Vector2f> &junctionPoints) {
	
    for (int i{ 0 }; i < junctionPoints.size(); i += 4) {

        //debugShape.setFillColor(sf::Color(0, 155, 155, 100));
        //debugShape.setPosition(junctionPoints[i + 1]); // side intersection point
        //window.draw(debugShape);
        //debugShape.setFillColor(sf::Color(155, 0, 155, 100));
        //debugShape.setPosition(junctionPoints[i]); // side intersection mirror point
        //window.draw(debugShape);
        //debugShape.setFillColor(sf::Color::Red);
        //debugShape.setPosition(junctionPoints[i + 2]); // road crossing point 1
        //window.draw(debugShape);
        //debugShape.setFillColor(sf::Color::Green);
        //debugShape.setPosition(junctionPoints[i + 3]); // road crossing point 2
        //window.draw(debugShape);

        sf::Vector2f v0 = junctionPoints[i + 1]; // side intersection point
        sf::Vector2f v1 = junctionPoints[i];     // side intersection mirror point
        sf::Vector2f v2 = junctionPoints[i + 3]; // road crossing point 1
        sf::Vector2f v3 = junctionPoints[i + 2]; // road crossing point 2

        vertices[0].position = v0;
        vertices[1].position = v1;
        vertices[2].position = v2;
        vertices[0].texCoords = { 0.f,   0.f };
        vertices[1].texCoords = { 256.f,   0.f };
        vertices[2].texCoords = { 256.f, 128.f };

        vertices[3].position = v0;
        vertices[4].position = v2;
        vertices[5].position = v3;
        vertices[3].texCoords = { 0.f,   0.f };
        vertices[4].texCoords = { 256.f, 128.f };
        vertices[5].texCoords = { 0.f, 128.f };

        window.draw(vertices, states);
    }
}


