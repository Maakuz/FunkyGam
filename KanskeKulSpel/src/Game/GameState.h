#pragma once

class GameState
{
public:
    enum class States
    {
        hub,
        level
    };

    GameState();
    virtual ~GameState() {};

    void operator=(States state) { GameState::state = state; };
    bool operator==(States state) { return GameState::state == state; };

    void setState(States state) { GameState::state = state; };

    static States getState() { return state; };

private:
    static States state;

};