#pragma once
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"
#include <vector>
#include <string>

struct BossSpawner
{
    sf::Vector2f pos;
    sf::FloatRect playerPosCriteria;
    std::vector<std::string> playerItemCriteria;
    int bossID;

    BossSpawner()
    {
        bossID = 0;
    }
};