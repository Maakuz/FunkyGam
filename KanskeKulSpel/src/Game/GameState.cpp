#include "GameState.h"

GameState::States GameState::state;

GameState::GameState()
{
    GameState::state = States::hub;
}
