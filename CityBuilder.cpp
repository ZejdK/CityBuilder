


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



void findAndSetHoveredJunction(sf::Vector2i cursorPos2i, const ConfigGlobal& config, const RoadNetwork& roadNetwork, EditorUI& editorUi, UIRenderer& uiRenderer);



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
                    findAndSetHoveredJunction(mouseButtonPressed->position, config, roadNetwork, editorUi, uiRenderer); // prevents no vertex being hovered after a new vertex is added and mouse is not moved
                }
            }
            else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) 
                findAndSetHoveredJunction(mouseMoved->position, config, roadNetwork, editorUi, uiRenderer);
        }



        citizenSimulation.update(dt * simulationSpeed);

        window.clear(sf::Color(0, 40, 0));
        roadRenderer.render(window, roadNetwork, cityView);
        uiRenderer.render(window, editorUi);
        citizenRenderer.render(window, citizenSimulation);
        window.display();
    }
}



void findAndSetHoveredJunction(sf::Vector2i cursorPos2i, const ConfigGlobal& config, const RoadNetwork& roadNetwork, EditorUI& editorUi, UIRenderer& uiRenderer) {

    sf::Vector2f cursorPos{ sf::Vector2f(cursorPos2i) };
    auto hoveredJunction{ roadNetwork.findJunctionNear(config.snapRadius, cursorPos) };

    uiRenderer.setCursorPos(cursorPos);
    editorUi.setHoveredJunction(hoveredJunction);
}


