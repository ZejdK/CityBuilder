


#pragma once
#include <optional>
#include <SFML/System/Vector2.hpp>
#include <cmath>



namespace CB { namespace Math {
    


    inline float distanceSq(const sf::Vector2f& a, const sf::Vector2f& b) {

        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    inline float distance(const sf::Vector2f& a, const sf::Vector2f& b) {

        return std::sqrt(distanceSq(a, b));
    }

    inline bool equals(const sf::Vector2f& a, const sf::Vector2f& b) {

        return distance(a, b) < 1e-10;
    }

    inline float cross(const sf::Vector2f &a, const sf::Vector2f &b) {

        return a.x * b.y - a.y * b.x;
    }

    inline std::optional<sf::Vector2f> intersect(const sf::Vector2f& a1, const sf::Vector2f& a2, const sf::Vector2f& b1, const sf::Vector2f& b2) {

        sf::Vector2f r = a2 - a1;
        sf::Vector2f s = b2 - b1;

        float denominator = cross(r, s);

        if (std::abs(denominator) < 1e-6f) // parallel
            return std::nullopt;

        sf::Vector2f diff = b1 - a1;

        float t = cross(diff, s) / denominator;
        float u = cross(diff, r) / denominator;

        if (t < 0.f || t > 1.f || u < 0.f || u > 1.f) 
            return std::nullopt;

        return a1 + r * t;
    }
}
}


