


#include <iostream>
#include <SFML/Graphics.hpp>
#include <boost/graph/adjacency_list.hpp>



void processEvent(std::optional<sf::Event> event, sf::RenderWindow& window, sf::CircleShape &pointer);
sf::Text createText(const sf::Font &font);



int main()
{
    boost::adjacency_list<> graph(5);
    add_edge(0, 1, graph);
    add_edge(1, 2, graph);

    sf::Texture textureTest("assetstemp/test.png");
    const sf::Texture textureTest2("assetstemp/test.gif");

    

    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "SFML works!"); // , sf::Style::Default, sf::State::Fullscreen);
    window.setFramerateLimit(60); // call it once after creating the window // these two don't mix
    // window.setKeyRepeatEnabled(false); smooth movement with events - boolean set on KeyPressed and clear on KeyReleased, easier solution is sf::Keyboard

    sf::Font font("assetstemp/arial.ttf");
    sf::Text text = createText(font);
    text.setString("Editor mode: add roads");
    bool addingRoad { false };
    sf::Vector2f firstNode{ 0.0f, 0.0f };
    sf::Vector2f secondNode{ 0.0f, 0.0f };
    
    sf::CircleShape pointer(5.0f);
    pointer.setFillColor(sf::Color::Red);

    std::array testline =
    {
        sf::Vertex{sf::Vector2f(10.f, 10.f)},
        sf::Vertex{sf::Vector2f(150.f, 150.f)}
    };

    using RoadElement = std::pair<sf::Vector2f, sf::Vector2f>;
    std::vector<RoadElement> roads{};


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
            }
            else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    auto pressedPosition{ sf::Vector2f(mouseButtonPressed->position) };
                    if (!addingRoad) {

                        firstNode = pressedPosition;
                        text.setString("Editor mode: adding a road");
                        addingRoad = true;
                    }
                    else {

                        roads.push_back({ firstNode, pressedPosition });
                        text.setString("Editor mode: add roads");
                        addingRoad = false;
                    }
                }
            }
            else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
            {
                secondNode = sf::Vector2f(mouseMoved->position);
                pointer.setPosition(sf::Vector2f(mouseMoved->position));
            }
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
        window.draw(text);
        window.draw(pointer);
        window.draw(testline.data(), testline.size(), sf::PrimitiveType::Lines);

        if (addingRoad) {

            std::array temp = { sf::Vertex{firstNode}, sf::Vertex{secondNode} };
            window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
        }
        for (const auto& road : roads) {

            std::array temp = { sf::Vertex{road.first}, sf::Vertex{road.second} };
            window.draw(temp.data(), temp.size(), sf::PrimitiveType::Lines);
        }

        window.display();
    }
}



sf::Text createText(const sf::Font &font) {

    sf::Text text(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Cyan);
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);

    return text;
}


