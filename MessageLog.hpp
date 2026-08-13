


#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics/Text.hpp>
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"



class MessageLog {

    std::vector<std::string> messages;
    sf::Font font;
    sf::Text textDisplay;

    float startX;
    float startY;
    float lineSpacing;

public:
    
    MessageLog(float x, float y, float spacing)
        : font("assetstemp/arial.ttf"), textDisplay(font), startX(x), startY(y), lineSpacing(spacing) {
        
        textDisplay.setFont(font);
        textDisplay.setCharacterSize(24);
        textDisplay.setFillColor(sf::Color::White);
        textDisplay.setStyle(sf::Text::Bold);
    }

    void add(const std::string& message) {

        messages.push_back(message);
    }

    void setColor(const sf::Color& color) {

        textDisplay.setFillColor(color);
    }

    void render(sf::RenderWindow& window) {

        float currentY = startY;

        for (const auto& message : messages) {

            textDisplay.setString(message);
            textDisplay.setPosition(sf::Vector2f(startX, currentY));

            window.draw(textDisplay);
            currentY += lineSpacing;
        }
    }

    void clear() {

        messages.clear();
    }
};


