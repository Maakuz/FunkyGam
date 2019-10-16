#pragma once
#include "MovingEntity.h"
#include "Misc/Counter.h"
#include "Handlers/UIHandler.h"

class Player : public MovingEntity 
{
public:
    Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos = sf::Vector2f(0, 0));
    ~Player() {};

    void update(float dt, sf::Vector2f mousePos);

    virtual void handleCollision(const Entity&  collider);

private:
    Counter platformPassingCounter;
    UIHandler* ui;
    bool debugMode;
    int selectedItemBarItem;

    void move(float dt);
    void debugMove(float dt);

};