


#include <iostream>
#include <SFML/Graphics.hpp>
#include "RoadNetwork.hpp"
#include "EditorUI.hpp"
#include "CityView.hpp"
#include "RoadRenderer.hpp"
#include "UIRenderer.hpp"
#include "Config.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/VideoMode.hpp"
#include "CitizenSimulation.hpp"
#include "SFML/System/Clock.hpp"
#include "CitizenRenderer.hpp"



void debugCreateSampleRoadNetwork(RoadNetwork &roadNetwork);
void findAndSetHoveredRoadElement(sf::Vector2i cursorPos2i, const ConfigGlobal& config, const RoadNetwork& roadNetwork, EditorUI& editorUi, UIRenderer& uiRenderer);



int main() {

    sf::RenderWindow window(sf::VideoMode({ 1800, 1000 }), "Zejd's City Builder - alpha ver"); // , sf::Style::Default, sf::State::Fullscreen);
    window.setFramerateLimit(60); // call it once after creating the window // these two don't mix
    // window.setKeyRepeatEnabled(false); smooth movement with events - boolean set on KeyPressed and clear on KeyReleased, easier solution is sf::Keyboard



    ConfigGlobal config {};
    float simulationSpeed { 1.f }; // 0 could be paused

    RoadNetwork roadNetwork {};
    CitizenSimulation citizenSimulation { roadNetwork };
    EditorUI editorUi { roadNetwork };

    RoadRenderer roadRenderer {};
    CitizenRenderer citizenRenderer {};
    UIRenderer uiRenderer { config };
    CityView cityView { CityView::Road };

    sf::Clock clock;

	debugCreateSampleRoadNetwork(roadNetwork);

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {

                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Q)
                    std::cout << "Cycling to next editor mode!",
                    editorUi.cycleMode();
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Num1)
                    std::cout << "Switched to road view of the city!\n",
                    cityView = CityView::Road;
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Num2)
                    std::cout << "Switched to graph only view of the city!\n",
                    cityView = CityView::Graph;
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Num3)
                    std::cout << "Switched to line only view of the city!\n",
                    cityView = CityView::Line;
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Num4)
                    std::cout << "Switched to all view of the city!\n",
                    cityView = CityView::RoadLineGraph;
            }
            else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {

                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {

                    editorUi.selectJunctionOrPos(sf::Vector2f(mouseButtonPressed->position));
                    findAndSetHoveredRoadElement(mouseButtonPressed->position, config, roadNetwork, editorUi, uiRenderer); // prevents no vertex being hovered after a new vertex is added and mouse is not moved
                }
            }
            else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) 
                findAndSetHoveredRoadElement(mouseMoved->position, config, roadNetwork, editorUi, uiRenderer);
        }



        citizenSimulation.update(dt * simulationSpeed);

        window.clear(sf::Color(0, 40, 0));
        roadRenderer.render(window, roadNetwork, cityView);
        uiRenderer.render(window, editorUi);
        citizenRenderer.render(window, citizenSimulation);
        window.display();
    }
}



void debugCreateSampleRoadNetwork(RoadNetwork& roadNetwork) {

    auto [ v1, v2 ] = roadNetwork.add(sf::Vector2f(100, 390), sf::Vector2f(261.f, 213.f));
    auto v3 = roadNetwork.add(v2, sf::Vector2f(500, 100)).second;
    auto v4 = roadNetwork.add(v3, sf::Vector2f(500, 450)).second;
    auto v5 = roadNetwork.add(v4, sf::Vector2f(500, 900)).second;
    auto v6 = roadNetwork.add(v5, sf::Vector2f(100, 900)).second;
    roadNetwork.add(v6, v1);

    auto [ v7, v8 ] = roadNetwork.add(sf::Vector2f(1700, 450), sf::Vector2f(1500, 900));
    auto v9 = roadNetwork.add(v8, sf::Vector2f(1100, 900)).second;
    auto v10 = roadNetwork.add(v9, sf::Vector2f(1100, 450)).second;
    auto v11 = roadNetwork.add(v10, sf::Vector2f(1100, 100)).second;
    auto v12 = roadNetwork.add(v11, sf::Vector2f(1600, 100)).second;
    roadNetwork.add(v12, v7);

    roadNetwork.add(v3, v11);
    roadNetwork.add(v4, v10);
    roadNetwork.add(v5, v9);
    roadNetwork.add(v10, v7);
}

void findAndSetHoveredRoadElement(sf::Vector2i cursorPos2i, const ConfigGlobal& config, const RoadNetwork& roadNetwork, EditorUI& editorUi, UIRenderer& uiRenderer) {

    sf::Vector2f cursorPos{ sf::Vector2f(cursorPos2i) };
    auto hoveredJunction{ roadNetwork.findJunctionNear(config.snapRadius, cursorPos) };
    auto hoveredEdge { roadNetwork.findHoveredRoad(config.roadWidth, cursorPos) };

    uiRenderer.setCursorPos(cursorPos);
    editorUi.setHoveredRoadElement(hoveredJunction, hoveredEdge);
}


