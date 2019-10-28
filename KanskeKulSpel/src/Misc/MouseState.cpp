#include "MouseState.h"

std::map<sf::Mouse::Button, bool> MOUSE::MouseState::prevButtonState;
bool MOUSE::MouseState::sameFrame = false;

bool MOUSE::MouseState::isButtonClicked(sf::Mouse::Button button)
{
    if (sf::Mouse::isButtonPressed(button))
    {

        if (prevButtonState.count(button))
        {
            if (!prevButtonState[button])
            {
                prevButtonState.insert_or_assign(button, true);
                sameFrame = true;
                return true;
            }

            else if (prevButtonState[button] && sameFrame)
                return true;

            else
                return false;
        }

        prevButtonState.insert_or_assign(button, true);
        sameFrame = true;
        return true;
    }

    return false;
}

void MOUSE::MouseState::updateButtons()
{
    sameFrame = false;
    for (auto const& val : prevButtonState)
    {
        if (val.second)
            if (!sf::Mouse::isButtonPressed(val.first))
                prevButtonState[val.first] = false;
    }
}
