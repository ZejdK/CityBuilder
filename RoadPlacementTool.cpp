


#include "RoadPlacementTool.hpp"
#include "SFML/Graphics/Vertex.hpp"



RoadPlacementTool::RoadPlacementTool()
        : font("assetstemp/arial.ttf"), currentModeText(font), pointer(5.0f) {

    currentModeText.setCharacterSize(24);
    currentModeText.setFillColor(sf::Color::Cyan);
    currentModeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    currentModeText.setString("Editor mode: add new node");

    pointer.setFillColor(sf::Color::Red);
}

bool RoadPlacementTool::selected() const {

    return selectedNode.has_value();
}

void RoadPlacementTool::selectNode(const sf::Vector2f& n) {

    currentModeText.setString("Editor mode: selecting second vertex");
    selectedNode = n;
}

RoadElement RoadPlacementTool::commitRoad(const sf::Vector2f& n) {

    currentModeText.setString("Editor mode: add new node");
    RoadElement roadSeg{ *selectedNode, n };

    selectedNode.reset();
    return roadSeg;
}

void RoadPlacementTool::deselectNode() {

    currentModeText.setString("Editor mode: add new node");
    selectedNode.reset();
}

void RoadPlacementTool::setHoverPointer(const sf::Vector2f& pos)
{

    pointer.setPosition(pos);
}

void RoadPlacementTool::draw(sf::RenderWindow& window) const {

    window.draw(currentModeText);

    if (selectedNode.has_value()) {

        // duplicated code, TODO: RoadRenderer
        std::array temp = { sf::Vertex{*selectedNode}, sf::Vertex{pointer.getPosition()} };
        window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
    }
    else {

        window.draw(pointer);
    }
}



