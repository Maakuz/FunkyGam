#pragma once
#include "Game/Particles/Emitter.h"

class SpellComp
{
public:
    SpellComp(const sf::Vector2f* pos);
    ~SpellComp() {};

    void startChannelling(int tomeID);
    void stopChannelling();
    void update(float dt);
    bool isChannelling() const { return channelling; };
    void castSpell(sf::Vector2f dest);

private:
    Emitter* channelEmitter;
    bool channelling;
    float channelTime;
    int tome;

    const sf::Vector2f* pos;

    void killEmitter();
};