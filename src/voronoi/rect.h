#pragma once
#include <SFML/Graphics.hpp>

struct Rectangle {
    float x{};
    float y{};
    float w{};
    float h{};

    sf::Vector2f iPos{};
    sf::Vector2f iVel{};

    bool checkPointCollision(sf::Vector2f position) {
        return position.x >= x and position.x < (x + w) and position.y >= y and position.y < (y + h);
    }

    void update() {
        iPos += iVel;

        if (iPos.x <= x or iPos.x >= x + w)
            iVel.x *= -1;

        if (iPos.y <= y or iPos.y >= y + h)
            iVel.y *= -1;
    }
};