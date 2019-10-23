#pragma once
#include "MovingEntity.h"
#include "Misc/Counter.h"
#include "Handlers/UIHandler.h"
#include <istream>

class Player : public MovingEntity 
{
public:
    struct inventory 
    {
        static const int QUICKSLOT_COUNT = 5;
        int quckslotItems[QUICKSLOT_COUNT];
    };
    Player(AnimationData data, UIHandler* uiHandler, sf::Vector2f pos = sf::Vector2f(0, 0));
    ~Player() {};

    friend std::istream& operator>>(std::istream& in, Player& player);

    void update(float dt, sf::Vector2f mousePos);

    void setIllumination(float illumination) { this->illumination = illumination; };
    float getIllumination() const { return illumination; };

    virtual void handleCollision(const Entity* collider);
    virtual void handleExplosion(const Explosion& explosion) {};

private:
    Counter platformPassingCounter;
    UIHandler* ui;
    int selectedItemBarItem;
    inventory inventory;
    float illumination;
    int health;

    bool debugMode;
    bool noClip;

    void move(float dt);
    void debugMove(float dt);

};