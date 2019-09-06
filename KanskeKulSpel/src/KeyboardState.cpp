#include "KeyboardState.h"

std::map<sf::Keyboard::Key, bool> KEYBOARD::KeyboardState::prevKeyState;

bool KEYBOARD::KeyboardState::isKeyClicked(sf::Keyboard::Key key)
{
    if (sf::Keyboard::isKeyPressed(key))
    {

        if (prevKeyState.count(key))
        {
            if (!prevKeyState[key])
            {
                prevKeyState.insert_or_assign(key, true);
                return true;
            }

            else
                return false;
        }

        prevKeyState.insert_or_assign(key, true);
        return true;
    }

    return false;
}

void KEYBOARD::KeyboardState::updateKeys()
{
    for (auto const& val : prevKeyState)
    {
        if (val.second)
            if (!sf::Keyboard::isKeyPressed(val.first))
                prevKeyState[val.first] = false;
    }
}
