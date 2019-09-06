#pragma once

#include <map>
#include "SFML/Window/Keyboard.hpp"

namespace KEYBOARD
{
    class KeyboardState
    {
    private:
        KeyboardState() {};
        ~KeyboardState() {};
        static std::map<sf::Keyboard::Key, bool> prevKeyState;

    public:
        static bool isKeyClicked(sf::Keyboard::Key key);

        static void updateKeys();
    };
}
