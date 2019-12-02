#include "SpellComp.h"
#include "Game/Particles/ParticleHandler.h"

SpellComp::SpellComp()
{
    this->channelTime = 0;
    this->channelling = false;
    this->channelEmitter = nullptr;
    this->spellID = 0;
}

void SpellComp::startChannelling(int id, int emitterID)
{
    this->channelling = true;
    this->spellID = id;
    this->channelTime = 0;

    this->channelEmitter = ParticleHandler::addEmitter(emitterID, pos);
}

void SpellComp::stopChannelling()
{
    this->channelling = false;
    killEmitter();
}

void SpellComp::update(float dt, sf::Vector2f pos)
{
    if (channelling)
        this->channelTime += dt;

    if (channelEmitter)
        channelEmitter->setEmitterPos(pos);

    this->pos = pos;
}

void SpellComp::killEmitter()
{
    if (channelEmitter)
    {
        channelEmitter->killQuick();
        channelEmitter = nullptr;
    }
}
