


#include "imgui.h"
#include "imgui-SFML.h"
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



void debugCreateSampleRoadNetwork(RoadNetwork& roadNetwork, CitizenSimulation& citizenSimulation);
void findAndSetHoveredRoadElement(sf::Vector2i cursorPos2i, const ConfigGlobal& config, const RoadNetwork& roadNetwork, const CitizenSimulation& citizenSimulation, EditorUI& editorUi, UIRenderer& uiRenderer);

void renderVehiclePaths(EditorUI& editorUi, CitizenRenderer& citizenRenderer, sf::RenderWindow& window, RoadNetwork& roadNetwork, CitizenSimulation& citizenSimulation);



int main() {

    ImVec2 roadInfoDefaultPos{ 1300.f ,100.f };
    sf::RenderWindow window(sf::VideoMode({ 1800, 1000 }), "Zejd's City Builder - alpha ver"); // , sf::Style::Default, sf::State::Fullscreen);
    window.setFramerateLimit(60); // call it once after creating the window // these two don't mix
    // window.setKeyRepeatEnabled(false); smooth movement with events - boolean set on KeyPressed and clear on KeyReleased, easier solution is sf::Keyboard

    if (!ImGui::SFML::Init(window))
        return -1;

    ImGui::GetIO().FontGlobalScale = 1.f;
    ImGui::GetStyle().ScaleAllSizes(1.f);



    ConfigGlobal config {};
    float simulationSpeed { 1.f }; // 0 could be paused

    RoadNetwork roadNetwork { config };
    CitizenSimulation citizenSimulation { roadNetwork, config };
    EditorUI editorUi { roadNetwork, citizenSimulation };

    RoadRenderer roadRenderer { config };
    CitizenRenderer citizenRenderer {};
    UIRenderer uiRenderer { config };
    CityView cityView { CityView::Road };

    double totalTime{ 0.0 };
    sf::Clock clock;

	debugCreateSampleRoadNetwork(roadNetwork, citizenSimulation);

    while (window.isOpen())
    {
		auto time{ clock.restart() };
        float dt = time.asSeconds();
        totalTime += dt;

        while (const std::optional event = window.pollEvent()) {

            ImGui::SFML::ProcessEvent(window, *event);

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

                if (!ImGui::GetIO().WantCaptureMouse && mouseButtonPressed->button == sf::Mouse::Button::Left) {

                    editorUi.selectJunctionOrPos(sf::Vector2f(mouseButtonPressed->position));
                    findAndSetHoveredRoadElement(mouseButtonPressed->position, config, roadNetwork, citizenSimulation, editorUi, uiRenderer); // prevents no vertex being hovered after a new vertex is added and mouse is not moved
                }
                if (mouseButtonPressed->button == sf::Mouse::Button::Right) {

                    auto hoveredJunction{ editorUi.getHoveredJunction() };
                    auto vss{ citizenSimulation.getVehicleSourceSinkAt(hoveredJunction) };

                    editorUi.requestVehicleSourceSinkPopup(hoveredJunction, vss);
                }
            }
            else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) 
                findAndSetHoveredRoadElement(mouseMoved->position, config, roadNetwork, citizenSimulation, editorUi, uiRenderer);
        }

        ImGui::SFML::Update(window, time);

		citizenSimulation.update(float(totalTime), dt * simulationSpeed);
        
        bool renderedVehicle{ uiRenderer.renderCitizenInfoImgui(editorUi) };
        if (!renderedVehicle)
            uiRenderer.renderRoadElementInfoImgui(editorUi, roadInfoDefaultPos);
        
        window.clear(sf::Color(0, 40, 0));
        roadRenderer.render(window, roadNetwork.getLayout(), roadNetwork.getGraph(), citizenSimulation.getVehicleSourceSinks(), cityView);
        citizenRenderer.render(window, citizenSimulation, config.roadWidth);
        renderVehiclePaths(editorUi, citizenRenderer, window, roadNetwork, citizenSimulation);
        uiRenderer.render(window, editorUi, roadNetwork.getLayout(), citizenSimulation);
        uiRenderer.renderVehicleSourceSinkPopupImgui(editorUi, citizenSimulation);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}



void debugCreateSampleRoadNetwork(RoadNetwork& roadNetwork, CitizenSimulation &citizenSimulation) {

    auto [ v1, v2 ] = roadNetwork.add(sf::Vector2f(100, 390), sf::Vector2f(261.f, 213.f));
    auto v3 = roadNetwork.add(v2, sf::Vector2f(540, 100)).second;
    auto v4 = roadNetwork.add(v3, sf::Vector2f(500, 450)).second;
    auto v5 = roadNetwork.add(v4, sf::Vector2f(460, 900)).second;
    auto v6 = roadNetwork.add(v5, sf::Vector2f(100, 880)).second;
    roadNetwork.add(v6, v1);

    auto [ v7, v8 ] = roadNetwork.add(sf::Vector2f(1700, 450), sf::Vector2f(1500, 900));
    auto v9 = roadNetwork.add(v8, sf::Vector2f(1100, 920)).second;
    auto v10 = roadNetwork.add(v9, sf::Vector2f(1100, 450)).second;
    auto v11 = roadNetwork.add(v10, sf::Vector2f(1100, 80)).second;
    auto v12 = roadNetwork.add(v11, sf::Vector2f(1600, 100)).second;
    roadNetwork.add(v12, v7);

    roadNetwork.add(v4, v10);
    roadNetwork.add(v5, v9);
    roadNetwork.add(v10, v7);

    auto vA = roadNetwork.add(v8, sf::Vector2f(1740, 890)).second;
    auto vB = roadNetwork.add(v3, sf::Vector2f(770, 90)).second;
    auto vC = roadNetwork.add(v11, sf::Vector2f(895, 90)).second;
    auto vD = roadNetwork.add(v4, sf::Vector2f(270, 440)).second;

    auto &roadLayout{ roadNetwork.getLayout() };
    citizenSimulation.addVehicleSourceSinkShortest(roadLayout.getJunction(v7), roadLayout.getJunction(v1), "r");
	citizenSimulation.addVehicleSourceSinkShortest(roadLayout.getJunction(vD), roadLayout.getJunction(vC), "g", 1.222f);
    citizenSimulation.addVehicleSourceSinkShortest(roadLayout.getJunction(vB), roadLayout.getJunction(vA), "b", 1.444f);
    citizenSimulation.addVehicleSourceSinkShortest(roadLayout.getJunction(v6), roadLayout.getJunction(vC), "o", 1.666f);
}

void findAndSetHoveredRoadElement(sf::Vector2i cursorPos2i, const ConfigGlobal& config, const RoadNetwork& roadNetwork, const CitizenSimulation &citizenSimulation, EditorUI& editorUi, UIRenderer& uiRenderer) {

    sf::Vector2f cursorPos{ sf::Vector2f(cursorPos2i) };

    auto hoveredVehicle{ citizenSimulation.findCitizen(cursorPos) };
    editorUi.setHoveredVehicle(hoveredVehicle);

    auto hoveredJunction{ roadNetwork.getLayout().findJunctionNear(config.snapRadius, cursorPos) };
    auto hoveredEdge { roadNetwork.getLayout().findHoveredRoad(config.roadWidth, cursorPos) };

    uiRenderer.setCursorPos(cursorPos);
    editorUi.setHoveredRoadElement(hoveredJunction, hoveredEdge);
}

void renderVehiclePaths(EditorUI& editorUi, CitizenRenderer& citizenRenderer, sf::RenderWindow& window, RoadNetwork& roadNetwork, CitizenSimulation& citizenSimulation) {

    auto selectedVehicle{ editorUi.getHoveredVehicle() };
    if (selectedVehicle)
        citizenRenderer.renderVehiclePath(window, roadNetwork, citizenSimulation, selectedVehicle->getPathId());

    auto hoveredJunction{ editorUi.getHoveredJunction() };
    auto vss{ citizenSimulation.getVehicleSourceSinkAt(hoveredJunction) };
    if (hoveredJunction && vss)
        citizenRenderer.renderVehiclePath(window, roadNetwork, citizenSimulation, vss->getPathId());
}


