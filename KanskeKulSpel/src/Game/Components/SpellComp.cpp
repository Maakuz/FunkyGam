#include "SpellComp.h"
#include "Game/Item/Projectile/ProjectileHandler.h"
#include "Game/Item/ItemHandler.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Item/Projectile/Spell/Tome.h"

SpellComp::SpellComp(const sf::Vector2f pos)
{
    this->channelTime = 0;
    this->channelling = false;
    this->tome = 0;
    this->channelEmitter = nullptr;

    this->pos = pos;
}

void SpellComp::startChannelling(int tomeID)
{
    this->channelling = true;
    this->tome = tomeID;
    this->channelTime = 0;

    int emitterID = dynamic_cast<const Tome*>(ItemHandler::getTemplate(tomeID))->getChannelEmitter();

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

void SpellComp::castSpell(sf::Vector2f dest)
{
    if (channelling)
        ProjectileHandler::addSpell(this->tome, this->pos, dest, this->channelTime);

    channelling = false;

    killEmitter();
}

void SpellComp::killEmitter()
{
    if (channelEmitter)
    {
        channelEmitter->killQuick();
        channelEmitter = nullptr;
    }
}
