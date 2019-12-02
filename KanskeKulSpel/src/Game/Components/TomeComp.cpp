#include "TomeComp.h"
#include "Game/Item/Projectile/ProjectileHandler.h"
#include "Game/Item/ItemHandler.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Item/Projectile/Spell/Tome.h"

TomeComp::TomeComp(const sf::Vector2f pos)
{
    this->channelTime = 0;
    this->channelling = false;
    this->tome = 0;
    this->channelEmitter = nullptr;

    this->pos = pos;
}

TomeComp::~TomeComp()
{
    this->killEmitter();
}

void TomeComp::startChannelling(int tomeID)
{
    this->channelling = true;
    this->tome = tomeID;
    this->channelTime = 0;

    int emitterID = dynamic_cast<const Tome*>(ItemHandler::getTemplate(tomeID))->getChannelEmitter();

    this->channelEmitter = ParticleHandler::addEmitter(emitterID, pos);
}

void TomeComp::stopChannelling()
{
    this->channelling = false;
    killEmitter();
}

void TomeComp::update(float dt, sf::Vector2f pos)
{
    if (channelling)
        this->channelTime += dt;

    if (channelEmitter)
        channelEmitter->setEmitterPos(pos);

    this->pos = pos;
}

void TomeComp::castSpell(sf::Vector2f dest)
{
    if (channelling)
    {
        const Tome* item = dynamic_cast<const Tome*>( ItemHandler::getTemplate(tome));

        if (item)
            ProjectileHandler::addSpell(item->getSpell(), this->pos, dest, this->channelTime);
        stopChannelling();
    }
}

void TomeComp::killEmitter()
{
    if (channelEmitter)
    {
        channelEmitter->killQuick();
        channelEmitter = nullptr;
    }
}
