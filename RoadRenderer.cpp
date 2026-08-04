


#include "RoadRenderer.hpp"
#include <SFML/System/Vector2.hpp>



RoadRenderer::RoadRenderer()
        : graphNodeShape(15.f), font("assetstemp/arial.ttf"), text(font), vertices(sf::PrimitiveType::Triangles, 6), states() {

    if (!texture.loadFromFile("assets/road_dotted.png"))
        throw "Unable to load the texture";

    if (!texturePlain.loadFromFile("assets/road_plain.png"))
        throw "Unable to load the texture: road_plain.png";

    texture.setRepeated(true);
    states.texture = &texture;

    text.setPosition(sf::Vector2f(10.f, 10.f));
    text.setCharacterSize(24);
    text.setFillColor(sf::Color(220, 170, 180));

    graphNodeShape.setOrigin(sf::Vector2f(15.f, 15.f));
    graphNodeShape.setFillColor(sf::Color(140, 0, 50));
}

void RoadRenderer::render(sf::RenderWindow& window, const RoadNetwork& roadNetwork, CityView cityView) {

    switch (cityView) {

    case CityView::Road:
        renderRoads(window, roadNetwork);
        renderIntersections(window, roadNetwork);
        break;
    case CityView::Graph:
        renderGraph(window, roadNetwork);
        break;
    case CityView::Line:
        // Intentionally left empty: line-only view handled elsewhere if needed
        break;
    case CityView::RoadLineGraph:
        renderRoads(window, roadNetwork);
        renderIntersections(window, roadNetwork);
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

        const RoadElement re { roadGraph[sourceVertex].position, roadGraph[targetVertex].position };
        renderRoad(window, re);
}
}

void RoadRenderer::renderRoad(sf::RenderWindow& window, const RoadElement &re) {

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



void RoadRenderer::renderIntersections(sf::RenderWindow& window, const RoadNetwork& roadNetwork) {

    auto roadGraph = roadNetwork.getGraph();
    auto [ begin, end ] = boost::vertices(roadGraph);

    states.texture = &texturePlain;
    for (auto it { begin }; it != end; ++it)
        renderIntersection(window, roadGraph, *it);
}

void RoadRenderer::renderIntersection(sf::RenderWindow& window, const RoadGraph &roadGraph, RoadVertexDescriptor vertex) { 

    auto [ begin, end ] = boost::out_edges(vertex, roadGraph);

    for (auto it { begin }; it != end; ++it) {

        EdgeVertexDescriptor edge = *it;
        auto targetVertex = boost::target(edge, roadGraph);
        
        const RoadElement re { roadGraph[vertex].position, roadGraph[targetVertex].position };
        std::array<sf::Vector2f, 4> road { re.getVertices(ROAD_WIDTH) };
        
        for (int i { 0 }; i < 4; ++i)
        {
            vertices[0].position = road[0];
            vertices[1].position = road[1];
            vertices[2].position = road[2];
            vertices[3].position = road[0];
            vertices[4].position = road[2];
            vertices[5].position = road[3];

            graphNodeShape.setPosition(road[i]);
            window.draw(graphNodeShape);
            // window.draw(vertices, states);
        }
    }
}


