#include "Fireball.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Misc/VectorFunctions.h"
#include "Game/Misc/Definitions.h"
#include "Game/Collision/CollisionHandler.h"
#include "Misc/ConsoleWindow.h"

Fireball::Fireball(sf::Vector2f pos)
:Spell(pos)
{
    this->damage = 0;
    this->maxTravelDistance = 0;
    this->minCharge = 0;
    this->maxCharge = 0;
    this->trailEmitterID = 0;
    this->impactEmitterID = 0;
    this->trail = nullptr;
    this->impacted = false;
    this->distance = 0;
    this->topSpeed = 0;

    addCollisionComponent(Collider::ColliderKeys::spell);
}

bool Fireball::isComplete() const
{
    return impacted;
}

void Fireball::cast(sf::Vector2f pos, sf::Vector2f dest)
{
    this->pos = pos;
    this->distance = std::min(length(pos - dest), maxTravelDistance);
    this->direction = dest - pos;
    normalize(this->direction);

    this->destination = pos + (direction * distance);
    trail = ParticleHandler::addEmitter(trailEmitterID, pos);
}

void Fireball::update(float dt)
{
    float t = std::max(0.f, 1 - (length(this->pos - this->destination) / distance));
    float velocity = (1 - powf(t, 6))* dt * topSpeed;
    pos += direction * velocity;
    trail->setEmitterPos(pos);

    if (velocity < 0.2f)
    {
        this->impacted = true;
        this->explosion.center = this->pos;
        trail->kill();
        CollisionHandler::queueExplosion(this->explosion);
        ParticleHandler::addEmitter(this->impactEmitterID, this->pos);
    }
}

void Fireball::handleCollision(const Entity* collider)
{

}

void Fireball::handleExplosion(const Explosion& explosion)
{
}

std::istream& Fireball::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> damage;
    in >> trash >> maxTravelDistance;
    in >> trash >> topSpeed;
    in >> trash >> minCharge;
    in >> trash >> maxCharge;
    in >> trash >> trailEmitterID;
    in >> trash >> impactEmitterID;
    in >> trash;
    in >> trash >> explosion.damage;
    in >> trash >> explosion.radius;
    in >> trash >> explosion.falloff;
    int type;
    in >> trash >> type;
    explosion.type = ExplosionType(type);

    return in;
}

std::ostream& Fireball::writeSpecific(std::ostream& out) const
{
    out << "[Specific]\n";
    out << "TickDamage: " << damage << "\n";
    out << "MaximumTravel: " << maxTravelDistance << "\n";
    out << "TopSpeed: " << topSpeed << "\n";
    out << "MinCharge: " << minCharge << "\n";
    out << "MaxCharge: " << maxCharge << "\n";
    out << "TrailEmitterID: " << trailEmitterID << "\n";
    out << "ImpactEmitterID: " << impactEmitterID << "\n";
    out << "[Explosion]" << "\n";
    out << "Damage: " << explosion.damage << "\n";
    out << "Radius: " << explosion.radius << "\n";
    out << "Falloff: " << explosion.falloff << "\n";
    out << "Type: " << (int)explosion.type << "\n";

    return out;
}
