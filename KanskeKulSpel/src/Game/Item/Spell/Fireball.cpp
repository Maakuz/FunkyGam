#include "Fireball.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"

Fireball::Fireball(sf::Vector2f pos)
:Spell(pos)
{
    this->damage = 0;
    this->maxTravelDistance = 0;
    this->radius = 0;
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
    this->destination = dest;

    this->distance = std::min(length(pos - dest), maxTravelDistance);
    this->direction = dest - pos;
    normalize(this->direction);

    trail = ParticleHandler::addEmitter(trailEmitterID, pos);
}

void Fireball::update(float dt)
{
    float velocity = length(this->pos - this->destination) / distance;
    printCon(std::to_string(velocity));
    pos += direction * velocity * dt * topSpeed;
    trail->setEmitterPos(pos);
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
    in >> trash >> radius;
    in >> trash >> minCharge;
    in >> trash >> maxCharge;
    in >> trash >> trailEmitterID;
    in >> trash >> impactEmitterID;

    return in;
}

std::ostream& Fireball::writeSpecific(std::ostream& out) const
{
    out << "[Specific]\n";
    out << "Damage: " << damage << "\n";
    out << "MaximumTravel: " << maxTravelDistance << "\n";
    out << "TopSpeed: " << topSpeed << "\n";
    out << "Radius: " << radius << "\n";
    out << "MinCharge: " << minCharge << "\n";
    out << "MaxCharge: " << maxCharge << "\n";
    out << "TrailEmitterID: " << trailEmitterID << "\n";
    out << "ImpactEmitterID: " << impactEmitterID << "\n";

    return out;
}
