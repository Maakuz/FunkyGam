#pragma once#include <vector>
#include <sstream>
#include "SFML/Graphics.hpp"
#include "Lighting/Light.h"

struct Particle
{
    sf::Vector2f velocity;
    sf::Color color;

    float lifespan;
    Light_NoShadow* light;
    Particle(sf::Vector2f velocity, sf::Color color, float life)
    {
        this->velocity = velocity;
        this->color = color;
        this->lifespan = life;
        this->light = nullptr;
    }
    Particle(const Particle& other)
    {
        this->light = nullptr;
        *this = other;
    }
    ~Particle()
    {
        delete light;
    }
    Particle& operator=(const Particle& other)
    {
        this->velocity = other.velocity;
        this->color = other.color;
        this->lifespan = other.lifespan;

        if (other.light != nullptr)
        {
            if (this->light)
                delete this->light;

            this->light = new Light_NoShadow(other.light->pos, other.light->radius, other.light->color);
        }
        else
        {
            if (this->light)
                delete this->light;

            this->light = nullptr;
        }
        return *this;
    }
};

class Emitter : public sf::Drawable
{
public:
    struct EmitterLight
    {
        sf::Vector2f offset;
        sf::Vector3f initialColor;
        Light* light;

        EmitterLight(sf::Vector2f emitterPos, float radius, sf::Vector3f color, sf::Vector2f offset = sf::Vector2f(0, 0))
        {
            light = new Light(emitterPos + offset, radius, color);
            this->offset = offset;
            this->initialColor = color;
        }

        EmitterLight(const EmitterLight& other)
        {
            *this = other;
        }

        ~EmitterLight()
        {
            delete light;
        }

        EmitterLight& operator=(const EmitterLight& other)
        {
            this->light = new Light(other.light->pos, other.light->radius, other.light->color);
            this->offset = other.offset;
            this->initialColor = other.initialColor;
            return *this;
        }
    };

    Emitter(sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f particleSize = sf::Vector2f(1, 1), sf::Color color = sf::Color::White, float spawnRate = 500, float particleSpeed = 1, float particleLife = 0, float emitterLife = 0, int initialParticles = 0, int particlesPerSpawn = 1, int startAngle = 0, int spread = 360, float frictionValue = 1, float jitterAmount = 0);
    Emitter(const Emitter& other);
    Emitter& operator=(const Emitter& other);
    ~Emitter();

    void update(float dt);

    friend std::ostream& operator<<(std::ostream& out, const Emitter& emitter);
    friend std::istream& operator>>(std::istream& in, Emitter& emitter);

    void setParticleLifeSpan(float lifespan);
    void setEmitterLifeSpan(float lifespan);

    void addLight(sf::Vector2f offset, float radius, sf::Vector3f color);
    std::vector<EmitterLight>* getLights() { return &this->lights; };

    int getNrOfParticles() const { return particles.size(); };

    void setSpeed(float speed) { this->speed = speed; };
    void setSpawnRate(float spawnRate) { this->spawnRate = spawnRate; };
    void setInitialParticles(int amount) { this->initialParticles = amount; };
    void setParticlesPerSpawn(int amount) { this->particlesPerSpawn = amount; };
    void setAngle(int angle) { this->emitterAngle = angle; };
    void setConeSize(int size) { this->emitterCone = size; };
    void setSize(sf::Vector2f size) { this->size = size; };
    void setColor(sf::Color color) { this->color = color; };
    void setColorDeviation(sf::Color color) { this->colorDeviation = color; };
    void setFriction(float value) { this->frictionValue = value; };
    void setJitter(float value) { this->jitterAmount = value; };
    void kill();

    //USE RESPONSIBLY
    void enableParticleLight();
    void setParticleLightRadius(float radius) { this->particleLightRadius = radius; };

    void enableGravity(bool isAffectedByGravity) { this->affectedByGravity = isAffectedByGravity; };

    void setEmitterPos(sf::Vector2f pos);
    sf::Vector2f getEmitterPos() const { return this->pos; };

    //When emitter stops producing particles, for when all particles are gone, see "isVeryDead"
    bool isDead() const { return this->emitterDead; };

    //Not the best name
    bool isVeryDead() const;

    void reset();
private:

    std::vector<EmitterLight> lights;

    sf::Vector2f pos;

    std::vector<Particle*> particles;
    std::vector<sf::Vertex> vertexArray;

    int initialParticles;
    int particlesPerSpawn;

    float lifespan;
    float lifespanCounter;

    float particleLifespan;
    float speed;

    float spawnRate;
    float spawnCounter;

    int emitterAngle;
    int emitterCone;

    sf::Vector2f size;
    sf::Color color;
    sf::Color colorDeviation;

    bool affectedByGravity;

    bool immortalEmitter;

    bool emitterDead;

    bool particlesHasLight; //HIGHLY EXPERIMENTAL MAYBE LEAVE THIS AT FALSE FOREVER
    float particleLightRadius;


    float jitterAmount;
    float frictionValue;


    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void addParticle();
};