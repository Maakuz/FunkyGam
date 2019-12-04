#pragma once
#include "Game/Particles/Emitter.h"
#include "Game/Item/Projectile/LightProjectile.h"
#include "Game/Item/Projectile/ProjectileHandler.h"

class SpellComp
{
public:
    SpellComp();
    virtual ~SpellComp() {};

    void startChannelling(int id, int emitterID);
    void stopChannelling();
    void update(float dt, sf::Vector2f pos);
    bool isChannelling() const { return channelling; };

    float getChannelTime() const { return channelTime; };

    template <typename SpellType>
    void castSpell(sf::Vector2f dest, DamageComp::DamageOrigin origin);

private:
    Emitter* channelEmitter;
    bool channelling;
    float channelTime;
    int spellID;

    sf::Vector2f pos;

    void killEmitter();
};

template<typename LightProjectile>
inline void SpellComp::castSpell(sf::Vector2f dest, DamageComp::DamageOrigin origin)
{
    sf::Vector2f dir = dest - pos;
    normalize(dir);
    ProjectileHandler::addProjectile(spellID, pos, dir, origin);
    stopChannelling();
}
