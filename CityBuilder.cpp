


#include <iostream>
#include <SFML/Graphics.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "RoadNetwork.hpp"
#include "RoadPlacementTool.hpp"
#include "CityBuilderTypes.hpp"




int main() {
    
    sf::Texture textureTest("assetstemp/test.png");
    const sf::Texture textureTest2("assetstemp/test.gif");

    CityView cityView { CityView::Line };
    

    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "SFML works!"); // , sf::Style::Default, sf::State::Fullscreen);
    window.setFramerateLimit(60); // call it once after creating the window // these two don't mix
    // window.setKeyRepeatEnabled(false); smooth movement with events - boolean set on KeyPressed and clear on KeyReleased, easier solution is sf::Keyboard

    std::array testline =
    {
        sf::Vertex{sf::Vector2f(10.f, 10.f)},
        sf::Vertex{sf::Vector2f(150.f, 150.f)}
    };

    RoadNetwork roadNetwork{};
    RoadPlacementTool roadPlacementTool{roadNetwork};



    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                ;// resized->size.x resized->size.y
            else if (event->is<sf::Event::FocusLost>())
                std::cout << "Focus lost sadge" << std::endl;
            else if (event->is<sf::Event::FocusGained>())
                std::cout << "Focus regained pepeyep" << std::endl;

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Num1)
                    std::cout << "Switched to line view of the city!\n",
                    cityView = CityView::Line;
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Num2)
                    std::cout << "Switched to graph view of the city!\n",
                    cityView = CityView::Graph;
            }
            else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {

                    if (!roadPlacementTool.selected())
                        roadPlacementTool.selectOriginPos();
                    else {
                        auto road = roadPlacementTool.commitRoad();
                        roadNetwork.add(road);
                    }
                }
            }
            else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
                roadPlacementTool.setCursorPos(sf::Vector2f(mouseMoved->position));
            else if (const auto* joystickButtonPressed = event->getIf<sf::Event::JoystickButtonPressed>())
            {
                std::cout << "joystick button pressed!" << std::endl;
                std::cout << "joystick id: " << joystickButtonPressed->joystickId << std::endl;
                std::cout << "button: " << joystickButtonPressed->button << std::endl;
            }
            else if (const auto* joystickMoved = event->getIf<sf::Event::JoystickMoved>())
            {
                if (joystickMoved->axis == sf::Joystick::Axis::X)
                {
                    std::cout << "X axis moved!" << std::endl;
                    std::cout << "joystick id: " << joystickMoved->joystickId << std::endl;
                    std::cout << "new position: " << joystickMoved->position << std::endl;
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            std::cout << "movin left :D";
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            std::cout << "movin right :D";

        if (sf::Joystick::isConnected(0))
        {
            float x = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X);
            float y = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y);
            //std::cout << "Controlla: " << x << "," << y << '\n';
        }

        window.clear();

        window.draw(testline.data(), testline.size(), sf::PrimitiveType::Lines);
        
        if (cityView == CityView::Line)
        roadNetwork.draw(window);
        else
            roadNetwork.drawGraph(window);

        roadPlacementTool.draw(window);

        window.display();
    }
}


