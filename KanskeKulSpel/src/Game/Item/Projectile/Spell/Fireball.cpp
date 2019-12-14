#include "Fireball.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Misc/VectorFunctions.h"
#include "Game/Misc/Definitions.h"
#include "Game/Collision/CollisionHandler.h"
#include "Misc/ConsoleWindow.h"

Fireball::Fireball(sf::Vector2f pos)
    :Spell(pos)
{
    m_maxTravelDistance = 0;
    m_minCharge = 0;
    m_maxCharge = 0;
    m_trailEmitterID = 0;
    m_impactEmitterID = 0;
    m_fullImpactEmitterID = 0;
    m_fullTrailEmitterID = 0;
    m_fullCharge = false;

    m_trail = nullptr;
    m_maxCharge = false;
    m_complete = false;
    m_distance = 0;
    m_topSpeed = 0;
    m_traveledDistance = 0;

    addComponent<DamageComp>(new DamageComp);

    getColliderComp()->addComponent(ColliderKeys::fireball);
}

Fireball::~Fireball()
{
    if (m_trail)
        ParticleHandler::destroyEmitter(m_trail);
}

bool Fireball::isComplete() const
{
    return m_complete;
}

void Fireball::cast(sf::Vector2f pos, sf::Vector2f dest, float channelTime)
{
    if (channelTime < m_minCharge)
        m_complete = true;

    else if (channelTime > m_minCharge && channelTime < m_maxCharge)
    {
        getTransformComp()->pos = pos;
        m_distance = std::min(length(pos - dest), m_maxTravelDistance);
        m_direction = dest - pos;
        normalize(m_direction);

        m_destination = pos + (m_direction * m_distance);
        m_trail = ParticleHandler::createEmitter(m_trailEmitterID, pos);
    }

    else
    {
        m_fullCharge = true;
        m_topSpeed *= 1.7f;
        getDamageComp()->damage *= 1.2f;
        m_maxTravelDistance *= 1.5f;

        m_explosion.damage *= 1.5f;
        m_explosion.radius *= 2.5f;

        getTransformComp()->pos = pos;
        m_distance = std::min(length(pos - dest), m_maxTravelDistance);
        m_direction = dest - pos;
        normalize(m_direction);

        m_destination = pos + (m_direction * m_distance);
        m_trail = ParticleHandler::createEmitter(m_fullTrailEmitterID, pos);
    }
}

void Fireball::update(float dt)
{
    TransformComp* transform = getTransformComp();
    float t = std::max(0.f, 1 - (length(transform->pos - m_destination) / m_distance));
    t = std::min(t, 1.f);

    float velocity = (1 - powf(t, 6)) * dt * m_topSpeed;

    transform->pos += m_direction * velocity;

    getColliderComp()->setPosition(transform->pos);
    m_trail->setEmitterPos(transform->pos);

    m_traveledDistance += velocity;

    if (m_trail)
        ParticleHandler::queueEmitter(m_trail);

    if (velocity < 0.2f || m_traveledDistance > m_distance)
    {
        m_complete = true;
        m_explosion.center = transform->pos;

        ParticleHandler::destroyEmitter(m_trail);
        m_trail = nullptr;

        CollisionHandler::queueExplosion(m_explosion);

        if (!m_fullCharge)
            ParticleHandler::addEmitter(m_impactEmitterID, transform->pos);

        else
            ParticleHandler::addEmitter(m_fullImpactEmitterID, transform->pos);
    }

}

void Fireball::handleCollision(const Collidable* collidable)
{
    const ColliderComp* otherCollider = collidable->getColliderComp();
    ColliderComp* collider = getColliderComp();

    if (otherCollider->hasComponent(ColliderKeys::ground))
    {
        if (ColliderComp::intersects(otherCollider->getLeftBox(), collider->getAABB()))
        {
            m_direction.x = -m_direction.x;

            float norm = otherCollider->left();
            m_destination.x = (norm * 2) - m_destination.x;
        }

        else if (ColliderComp::intersects(otherCollider->getRightBox(), collider->getAABB()))
        {
            m_direction.x = -m_direction.x;

            float norm = otherCollider->right();
            m_destination.x = (norm * 2) - m_destination.x;
        }

        else if (ColliderComp::intersects(otherCollider->getUpBox(), collider->getAABB()))
        {
            m_direction.y = -m_direction.y;

            float norm = otherCollider->up();
            m_destination.y = (norm * 2) - m_destination.y;
        }

        else if (ColliderComp::intersects(otherCollider->getDownBox(), collider->getAABB()))
        {
            m_direction.y = -m_direction.y;

            float norm = otherCollider->down();
            m_destination.y = (norm * 2) - m_destination.y;
        }
    }
}

void Fireball::handleExplosion(const Explosion& explosion)
{
}

std::istream& Fireball::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> getDamageComp()->damage;
    in >> trash >> m_maxTravelDistance;
    in >> trash >> m_topSpeed;
    in >> trash >> m_minCharge;
    in >> trash >> m_maxCharge;
    in >> trash >> m_trailEmitterID;
    in >> trash >> m_impactEmitterID;
    in >> trash >> m_fullTrailEmitterID;
    in >> trash >> m_fullImpactEmitterID;
    in >> m_explosion;

    return in;
}

std::ostream& Fireball::writeSpecific(std::ostream& out) const
{
    out << "[Specific]\n";
    out << "TickDamage: " << getDamageComp()->damage << "\n";
    out << "MaximumTravel: " << m_maxTravelDistance << "\n";
    out << "TopSpeed: " << m_topSpeed << "\n";
    out << "MinCharge: " << m_minCharge << "\n";
    out << "MaxCharge: " << m_maxCharge << "\n";
    out << "TrailEmitterID: " << m_trailEmitterID << "\n";
    out << "ImpactEmitterID: " << m_impactEmitterID << "\n";
    out << "FullTrailEmitterID: " << m_fullTrailEmitterID << "\n";
    out << "FullImpactEmitterID: " << m_fullImpactEmitterID << "\n";
    out << m_explosion << "\n";

    return out;
}
