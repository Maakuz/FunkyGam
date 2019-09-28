#pragma once#include <vector>
#include <sstream>
#include "SFML/Graphics.hpp"

struct Particle
{
    sf::Vector2f velocity;
    sf::Color color;

    float lifespan;
};

class Emitter : public sf::Drawable
{
public:
    Emitter(sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f particleSize = sf::Vector2f(1, 1), sf::Color color = sf::Color::White, float spawnRate = 500, float particleSpeed = 1, float particleLife = 0, float emitterLife = 0, int initialParticles = 0, int particlesPerSpawn = 1, int startAngle = 0, int spread = 360);
    ~Emitter() {};

    void update(float dt);

    friend std::ostream& operator<<(std::ostream& out, const Emitter& emitter);
    friend std::istream& operator>>(std::istream& in, Emitter& emitter);

    void setParticleLifeSpan(float lifespan);
    void setEmitterLifeSpan(float lifespan);

    void setSpeed(float speed) { this->speed = speed; };
    void setSpawnRate(float spawnRate) { this->spawnRate = spawnRate; };
    void setInitialParticles(int amount) { this->initialParticles = amount; };
    void setParticlesPerSpawn(int amount) { this->particlesPerSpawn = amount; };
    void setAngle(int angle) { this->emitterAngle = angle; };
    void setConeSize(int size) { this->emitterCone = size; };
    void setSize(sf::Vector2f size) { this->size = size; };
    void setColor(sf::Color color) { this->color = color; };

    void enableGravity(bool isAffectedByGravity) { this->affectedByGravity = isAffectedByGravity; };

    void setEmitterPos(sf::Vector2f pos) { this->pos = pos; };

    void reset();
private:
    sf::Vector2f pos;

    std::vector<Particle> particles;
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

    bool affectedByGravity;

    bool immortalParticles;
    bool immortalEmitter;

    bool emitterdead;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void addParticle();
};