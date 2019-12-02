#pragma once
#include "Game/Particles/Emitter.h"

class TomeComp
{
public:
    TomeComp(sf::Vector2f pos);
    ~TomeComp();

    void startChannelling(int tomeID);
    void stopChannelling();
    void update(float dt, sf::Vector2f pos);
    bool isChannelling() const { return channelling; };
    void castSpell(sf::Vector2f dest);

private:
    Emitter* channelEmitter;
    bool channelling;
    float channelTime;
    int tome;

    sf::Vector2f pos;

    void killEmitter();
};