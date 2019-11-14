#pragma once#include <vector>
#include <sstream>
#include "SFML/Graphics.hpp"
#include "Renderer/Lighting/Light.h"
#include "Game/Entities/Entity.h"

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
        sf::Vector2f initialOffset;
        sf::Vector3f initialColor;
        float initialRadius;
        Light* light;

        EmitterLight(sf::Vector2f emitterPos, float radius, sf::Vector3f color, sf::Vector2f offset = sf::Vector2f(0, 0))
        {
            light = new Light(emitterPos + offset, radius, color);
            this->offset = offset;
            this->initialOffset = offset;
            this->initialColor = color;
            this->initialRadius = radius;
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
            this->initialOffset = other.initialOffset;
            this->initialRadius = other.initialRadius;
            return *this;
        }
    };

    struct KeyFrame
    {
        float timeStamp;

        int particlesPerSpawn;
        float particleLifespan;
        float speed;
        float spawnRate;
        int emitterAngle;
        int emitterCone;
        sf::Vector2f size;
        sf::Color color;
        sf::Color colorDeviation;
        bool affectedByGravity;
        float gravity;
        float particleLightRadius;
        float jitterAmount;
        float frictionValue;
        std::vector<EmitterLight> lights;
        
        KeyFrame(float timeStamp = 0)
        {
            this->timeStamp = timeStamp;
            this->particleLifespan = 0;
            this->particleLightRadius = 0;
            this->particlesPerSpawn = 0;
            this->spawnRate = 0;
            this->speed = 0;
            this->emitterAngle = 0;
            this->emitterCone = 0;
            this->affectedByGravity = 0;
            this->gravity = 0;
            this->jitterAmount = 0;
            this->frictionValue = 0;
        }

        friend std::ostream& operator<<(std::ostream& out, const KeyFrame& frame)
        {
            out << frame.timeStamp << "\n";
            out << frame.particlesPerSpawn << "\n";
            out << frame.particleLifespan << "\n";
            out << frame.speed << "\n";
            out << frame.spawnRate << "\n";
            out << frame.emitterAngle << "\n";
            out << frame.emitterCone << "\n";
            out << frame.size.x << " " << frame.size.y << "\n";
            out << (int)frame.color.r << " " << (int)frame.color.g << " " << (int)frame.color.b << " " << (int)frame.color.a << "\n";
            out << (int)frame.colorDeviation.r << " " << (int)frame.colorDeviation.g << " " << (int)frame.colorDeviation.b << " " << (int)frame.color.a << "\n";
            out << frame.affectedByGravity << "\n";
            out << frame.gravity << "\n";
            out << frame.jitterAmount << "\n";
            out << frame.frictionValue << "\n";
            out << frame.particleLightRadius << "\n";
            out << frame.lights.size() << "\n";

            for (size_t i = 0; i < frame.lights.size(); i++)
            {
                out << frame.lights[i].offset.x << " " << frame.lights[i].offset.y << "\n";
                out << frame.lights[i].light->radius << "\n";
                out << frame.lights[i].initialColor.x << " " << frame.lights[i].initialColor.y << " " << frame.lights[i].initialColor.z << "\n";
            }

            return out;
        }
        
        friend std::istream& operator>>(std::istream& in, KeyFrame& frame)
        {
            int col[4] = {0};
            int lightCount = 0;
            in >> frame.timeStamp;
            in >> frame.particlesPerSpawn;
            in >> frame.particleLifespan;
            in >> frame.speed;
            in >> frame.spawnRate;
            in >> frame.emitterAngle;
            in >> frame.emitterCone;
            in >> frame.size.x >> frame.size.y;
            in >> col[0] >> col[1] >> col[2] >> col[3];
            frame.color = sf::Color(col[0], col[1], col[2], col[3]);
            in >> col[0] >> col[1] >> col[2] >> col[3];
            frame.colorDeviation = sf::Color(col[0], col[1], col[2], col[3]);
            in >> frame.affectedByGravity;
            in >> frame.gravity;
            in >> frame.jitterAmount;
            in >> frame.frictionValue;
            in >> frame.particleLightRadius;
            in >> lightCount;

            frame.lights.clear();
            for (int i = 0; i < lightCount; i++)
            {
                sf::Vector2f offset;
                float radius;
                sf::Vector3f color;
                in >> offset.x >> offset.y;
                in >> radius;
                in >> color.x >> color.y >> color.z;

                frame.lights.push_back(Emitter::EmitterLight(sf::Vector2f(), radius, color, offset));
            }
            return in;
        }
    };

    Emitter(sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f particleSize = sf::Vector2f(1, 1),
        sf::Color color = sf::Color::White, float spawnRate = 500, float particleSpeed = 1,
        float particleLife = 0, float emitterLife = 0, int initialParticles = 0, 
        int particlesPerSpawn = 1, int startAngle = 0, int spread = 360, 
        float frictionValue = 1, float jitterAmount = 0, float gravity = 0.1f,
        bool hasCollision = false);

    Emitter(const Emitter& other);
    Emitter& operator=(const Emitter& other);
    ~Emitter();

    void update(float dt);
    void queueLights();
    void handleCollision(const std::vector<Entity*>* entities);
    void kill();

    friend std::ostream& operator<<(std::ostream& out, const Emitter& emitter);
    friend std::istream& operator>>(std::istream& in, Emitter& emitter);

    void setEmitterLifeSpan(float lifespan);
    float getEmitterLifeSpan()const { return lifespan; };
    void setElapsedTime(float time);
    float getElapsedTime() const { return elapsedTime; };
    void setFreeze(bool freeze) { this->freezeTime = freeze; };

    KeyFrame* getKeyFramePtr(int frame) { return &keyFrames[frame]; };
    int getKeyFrameCount() const { return keyFrames.size(); };
    void addKeyFrame(int pos);
    void removeKeyFrame(int frame) { keyFrames.erase(keyFrames.begin() + frame); };

    int getParticleCount() const { return particles.size(); };

    void setInitialParticles(int amount) { this->initialParticles = amount; };

    void setParticleHasLight(bool hasLight);

    void setColliding(bool hasCollision) { this->colliding = hasCollision; };
    bool isColliding()const { return colliding; };

    void setEmitterPos(sf::Vector2f pos);
    sf::Vector2f getEmitterPos() const { return this->pos; };

    //When emitter stops producing particles, for when all particles are gone, see "isVeryDead"
    bool isDead() const { return this->emitterDead; };

    //Not the best name
    bool isVeryDead() const;

    void reset();
private:
    std::vector<KeyFrame> keyFrames;
    int prev;
    int next;

    float elapsedTime;

    sf::Vector2f pos;

    std::vector<Particle*> particles;
    std::vector<sf::Vertex> vertexArray;

    int initialParticles;

    float lifespan;

    float spawnCounter;

    bool immortalEmitter;
    bool colliding;

    bool emitterDead;
    bool freezeTime;

    bool particlesHasLight;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void addParticle(float lerp);
    void lerpLights(float lerp);
};