#include "SpellComp.h"
#include "Game/Particles/ParticleHandler.h"

SpellComp::SpellComp()
{
    this->channelTime = 0;
    this->channelling = false;
    this->channelEmitter = nullptr;
    this->spellID = 0;
}

SpellComp::~SpellComp()
{
    if (channelEmitter)
        ParticleHandler::destroyEmitter(channelEmitter, true);
}

void SpellComp::startChannelling(int id, int emitterID)
{
    this->channelling = true;
    this->spellID = id;
    this->channelTime = 0;

    this->channelEmitter = ParticleHandler::createEmitter(emitterID, pos);
}

void SpellComp::stopChannelling()
{
    this->channelling = false;
    if (channelEmitter)
    {
        ParticleHandler::destroyEmitter(channelEmitter, true);
        channelEmitter = nullptr;
    }
}

void SpellComp::update(float dt, sf::Vector2f pos)
{
    if (channelling)
        this->channelTime += dt;

    if (channelEmitter)
        channelEmitter->setEmitterPos(pos);

    this->pos = pos;
}