#include "Fireball.h"

Fireball::Fireball(sf::Vector2f pos, const sf::Texture* texture)
:Spell(pos, texture)
{
    this->damage = 0;
    this->maxTravelDistance = 0;
    this->radius = 0;
    this->minCharge = 0;
    this->maxCharge = 0;
    this->trailEmitterID = 0;
    this->impactEmitterID = 0;
}

bool Fireball::isComplete() const
{
    return false;
}

void Fireball::cast()
{
}

void Fireball::update(float dt)
{
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
    out << "Radius: " << radius << "\n";
    out << "MinCharge: " << minCharge << "\n";
    out << "MaxCharge: " << maxCharge << "\n";
    out << "TrailEmitterID: " << trailEmitterID << "\n";
    out << "ImpactEmitterID: " << impactEmitterID << "\n";

    return out;
}
