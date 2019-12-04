#include "Fireball.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Misc/VectorFunctions.h"
#include "Game/Misc/Definitions.h"
#include "Game/Collision/CollisionHandler.h"
#include "Misc/ConsoleWindow.h"

Fireball::Fireball(sf::Vector2f pos)
    :Spell(pos)
{
    this->maxTravelDistance = 0;
    this->minCharge = 0;
    this->maxCharge = 0;
    this->trailEmitterID = 0;
    this->impactEmitterID = 0;
    this->fullImpactEmitterID = 0;
    this->fullTrailEmitterID = 0;
    this->fullCharge = false;

    this->trail = nullptr;
    this->maxCharge = false;
    this->complete = false;
    this->distance = 0;
    this->topSpeed = 0;
    this->traveledDistance = 0;

    addComponent<DamageComp>(new DamageComp);

    getColliderComp()->addComponent(ColliderKeys::fireball);
}

bool Fireball::isComplete() const
{
    return complete;
}

void Fireball::cast(sf::Vector2f pos, sf::Vector2f dest, float channelTime)
{
    if (channelTime < this->minCharge)
        this->complete = true;

    else if (channelTime > this->minCharge && channelTime < this->maxCharge)
    {
        getTransformComp()->pos = pos;
        this->distance = std::min(length(pos - dest), maxTravelDistance);
        this->direction = dest - pos;
        normalize(this->direction);

        this->destination = pos + (direction * distance);
        trail = ParticleHandler::addEmitter(trailEmitterID, pos);
    }

    else
    {
        this->fullCharge = true;
        this->topSpeed *= 1.7f;
        getDamageComp()->damage *= 1.2f;
        this->maxTravelDistance *= 1.5f;

        this->explosion.damage *= 1.5f;
        this->explosion.radius *= 2.5f;

        getTransformComp()->pos = pos;
        this->distance = std::min(length(pos - dest), maxTravelDistance);
        this->distance = std::max(length(pos - dest), 64.f);
        this->direction = dest - pos;
        normalize(this->direction);

        this->destination = pos + (direction * distance);
        this->trail = ParticleHandler::addEmitter(fullTrailEmitterID, pos);
    }
}

void Fireball::update(float dt)
{
    TransformComp* transform = getTransformComp();
    float t = std::max(0.f, 1 - (length(transform->pos - this->destination) / distance));
    t = std::min(t, 1.f);
    float velocity = (1 - powf(t, 6)) * dt * topSpeed;
    transform->pos += direction * velocity;
    getColliderComp()->setPosition(transform->pos);
    trail->setEmitterPos(transform->pos);

    if (velocity < 0.2f)
    {
        this->complete = true;
        this->explosion.center = transform->pos;
        trail->kill();
        CollisionHandler::queueExplosion(this->explosion);

        if (!fullCharge)
            ParticleHandler::addEmitter(this->impactEmitterID, transform->pos);

        else
            ParticleHandler::addEmitter(this->fullImpactEmitterID, transform->pos);
    }

    this->traveledDistance += velocity;
    if (traveledDistance > maxTravelDistance * 5.f)
    {
        trail->kill();
        complete = true;
    }
}

void Fireball::handleCollision(const Collidable* collidable)
{

}

void Fireball::handleExplosion(const Explosion& explosion)
{
}

std::istream& Fireball::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> getDamageComp()->damage;
    in >> trash >> maxTravelDistance;
    in >> trash >> topSpeed;
    in >> trash >> minCharge;
    in >> trash >> maxCharge;
    in >> trash >> trailEmitterID;
    in >> trash >> impactEmitterID;
    in >> trash >> fullTrailEmitterID;
    in >> trash >> fullImpactEmitterID;
    in >> explosion;

    return in;
}

std::ostream& Fireball::writeSpecific(std::ostream& out) const
{
    out << "[Specific]\n";
    out << "TickDamage: " << getDamageComp()->damage << "\n";
    out << "MaximumTravel: " << maxTravelDistance << "\n";
    out << "TopSpeed: " << topSpeed << "\n";
    out << "MinCharge: " << minCharge << "\n";
    out << "MaxCharge: " << maxCharge << "\n";
    out << "TrailEmitterID: " << trailEmitterID << "\n";
    out << "ImpactEmitterID: " << impactEmitterID << "\n";
    out << "FullTrailEmitterID: " << fullTrailEmitterID << "\n";
    out << "FullImpactEmitterID: " << fullImpactEmitterID << "\n";
    out << explosion << "\n";

    return out;
}
