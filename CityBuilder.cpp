


#include <iostream>
#include <SFML/Graphics.hpp>
#include "RoadNetwork.hpp"
#include "RoadPlacementTool.hpp"
#include "CityBuilderTypes.hpp"



int main() {

    sf::RenderWindow window(sf::VideoMode({ 1800, 1000 }), "Zejd's City Builder - alpha ver"); // , sf::Style::Default, sf::State::Fullscreen);
    window.setFramerateLimit(60); // call it once after creating the window // these two don't mix
    // window.setKeyRepeatEnabled(false); smooth movement with events - boolean set on KeyPressed and clear on KeyReleased, easier solution is sf::Keyboard


    RoadNetwork roadNetwork {};
    RoadPlacementTool roadPlacementTool {roadNetwork};
    CityView cityView { CityView::Line };

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {

                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Num1)
                    std::cout << "Switched to line view of the city!\n",
                    cityView = CityView::Line;
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Num2)
                    std::cout << "Switched to graph view of the city!\n",
                    cityView = CityView::Graph;
            }
            else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {

                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {

                    if (!roadPlacementTool.selected())
                        roadPlacementTool.selectOriginPos();
                    else {
                        auto road = roadPlacementTool.commitRoad();
                        roadNetwork.add(road);
                    }
                }
            }
            else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {

                roadPlacementTool.setCursorPos(sf::Vector2f(mouseMoved->position));
            }
        }



        window.clear();
        
        if (cityView == CityView::Line)
            roadNetwork.draw(window);
        else
            roadNetwork.drawGraph(window);

        roadPlacementTool.draw(window);

        window.display();
    }
}


