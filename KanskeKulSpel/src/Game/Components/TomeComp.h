#pragma once
#include "Game/Particles/Emitter.h"
#include "Game/Components/Comp.h"

class TomeComp : public Comp
{
public:
    TomeComp(sf::Vector2f pos);
    virtual ~TomeComp();

    static ComponentKey getStaticKey() { return ComponentKey::tome; };
    virtual Comp* clone() const { return new TomeComp(*this); };

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