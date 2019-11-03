#pragma once

#include <map>
#include "SFML/Window/Mouse.hpp"

namespace MOUSE
{
    class MouseState
    {
    private:
        MouseState() {};
        ~MouseState() {};
        static std::map<sf::Mouse::Button, bool> prevButtonState;
        static bool sameFrame;

    public:
        static bool isButtonClicked(sf::Mouse::Button button);

        static void updateButtons();
    };
}
