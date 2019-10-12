#include "Emitter.h"
#include "Lighting/LightQueue.h"

Emitter::Emitter(sf::Vector2f pos, sf::Vector2f particleSize, sf::Color color, float spawnRate, float particleSpeed, float particleLife, float emitterLife, int initialParticles, int particlesPerSpawn, int startAngle, int spread)
{
    this->pos = pos;
    this->speed = particleSpeed;
    this->spawnRate = spawnRate;
    this->size = particleSize;
    this->particleLifespan = particleLife;
    this->color = color;
    this->initialParticles = initialParticles;
    this->particlesPerSpawn = particlesPerSpawn;
    this->emitterAngle = startAngle;
    this->emitterCone = spread;

    this->affectedByGravity = false;

    this->setEmitterLifeSpan(emitterLife);
    this->setParticleLifeSpan(particleLife);
    this->reset();

    emitterLight = new Light(pos, 100);
}

Emitter::Emitter(const Emitter& other)
{
    *this = other;
}

Emitter& Emitter::operator=(const Emitter& other)
{
    this->affectedByGravity = other.affectedByGravity;
    this->color = other.color;
    this->emitterAngle = other.emitterAngle;
    this->emitterCone = other.emitterCone;
    this->emitterDead = other.emitterDead;
    this->immortalEmitter = other.immortalEmitter;
    this->immortalParticles = other.immortalParticles;
    this->initialParticles = other.initialParticles;
    this->lifespan = other.lifespan;
    this->lifespanCounter = 0;
    this->particleLifespan = other.particleLifespan;
    this->particles = other.particles;
    this->particlesPerSpawn = other.particlesPerSpawn;
    this->pos = other.pos;
    this->size = other.size;
    this->spawnCounter = 0;
    this->spawnRate = other.spawnRate;
    this->speed = other.speed;
    this->vertexArray = other.vertexArray;

    this->emitterLight = new Light(other.emitterLight->pos, other.emitterLight->radius, other.emitterLight->color);
    return *this;
}

Emitter::~Emitter()
{
    delete this->emitterLight;
}

void Emitter::update(float dt)
{
    LightQueue::get().queue(emitterLight);

    this->lifespanCounter += dt;
    if (this->lifespanCounter > this->lifespan)
    {
        this->emitterDead = true;
        emitterLight->color.x *= 0.95f;
        emitterLight->color.y *= 0.95f;
        emitterLight->color.z *= 0.95f;
    }

    this->spawnCounter += dt;
    if (this->spawnCounter > this->spawnRate && !this->emitterDead)
    {
        this->spawnCounter = 0;

        for (int i = 0; i < particlesPerSpawn; i++)
            this->addParticle();

    }

    for (size_t i = 0; i < particles.size(); i++)
    {
        int p = (int)i * 4;
        Particle* particle = &particles[i];

        if (!this->immortalParticles)
            particle->lifespan -= dt;

        if (particle->lifespan < 0)
        {
            particles.erase(particles.begin() + i);
            vertexArray.erase(vertexArray.begin() + p, vertexArray.begin() + p + 4);
            
            i--;
        }

        else
        {
            if (this->affectedByGravity)
                particle->velocity.y += 0.01f * dt;

            for (size_t j = 0; j < 4; j++)
            {
                vertexArray[p + j].position += particle->velocity;
            }
        }
    }
}

void Emitter::setParticleLifeSpan(float lifespan)
{
    this->immortalParticles = false;
    this->particleLifespan = lifespan;
    if (this->particleLifespan <= 0.00023) //perfect number
    {
        this->particleLifespan = 0;
        this->immortalParticles = true;
    }
}

void Emitter::setEmitterLifeSpan(float lifespan)
{
    this->immortalEmitter = false;
    this->lifespan = lifespan;
    if (this->lifespan <= 0.00023) //perfect number
    {
        this->lifespan = 0;
        this->immortalEmitter = true;
    }
}

void Emitter::setEmitterPos(sf::Vector2f pos) 
{ 
    this->pos = pos; 
    this->emitterLight->pos = pos;
}

bool Emitter::isVeryDead() const
{
    return this->emitterDead && particles.empty();
}

void Emitter::reset()
{
    this->spawnCounter = 0;
    this->lifespanCounter = 0;
    this->emitterDead = false;

    this->particles.clear();
    this->vertexArray.clear();

    for (int i = 0; i < initialParticles; i++)
    {
        this->addParticle();
    }
}

void Emitter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!vertexArray.empty())
        target.draw(&vertexArray[0], vertexArray.size(), sf::Quads);
}

void Emitter::addParticle()
{
    float angle = ((std::rand() % emitterCone) + emitterAngle) * 3.14f / 180.f;
    float randSpeed = ((std::rand() % 25) / 100.f) + this->speed;
    Particle particle;
    particle.velocity = sf::Vector2f(randSpeed * std::cos(angle), randSpeed * std::sin(angle));
    particle.color = this->color;
    particle.lifespan = this->particleLifespan;

    particles.push_back(particle);

    vertexArray.push_back(sf::Vertex(this->pos + sf::Vector2f(-size.x, -size.y), particle.color));
    vertexArray.push_back(sf::Vertex(this->pos + sf::Vector2f(size.x, -size.y), particle.color));
    vertexArray.push_back(sf::Vertex(this->pos + sf::Vector2f(size.x, size.y), particle.color));
    vertexArray.push_back(sf::Vertex(this->pos + sf::Vector2f(-size.x, size.y), particle.color));

}

std::ostream& operator<<(std::ostream& out, const Emitter& emitter)
{
    out << emitter.pos.x << " " << emitter.pos.y << "\n";
    out << emitter.initialParticles << "\n";
    out << emitter.particlesPerSpawn << "\n";
    out << emitter.lifespan << "\n";
    out << emitter.particleLifespan << "\n";
    out << emitter.speed << "\n";
    out << emitter.spawnRate << "\n";
    out << emitter.emitterAngle << "\n";
    out << emitter.emitterCone << "\n";
    out << emitter.size.x << " " << emitter.size.y << "\n";
    out << (int)emitter.color.r << " " << (int)emitter.color.g << " " << (int)emitter.color.b << " " << (int)emitter.color.a << "\n";
    out << emitter.affectedByGravity << "\n";

    return out;
}

std::istream& operator>>(std::istream& in, Emitter& emitter)
{
    in >> emitter.pos.x >> emitter.pos.y;
    in >> emitter.initialParticles;
    in >> emitter.particlesPerSpawn;
    in >> emitter.lifespan;
    in >> emitter.particleLifespan;
    in >> emitter.speed;
    in >> emitter.spawnRate;
    in >> emitter.emitterAngle;
    in >> emitter.emitterCone;
    in >> emitter.size.x >> emitter.size.y;

    int color[4] = {0};

    in >> color[0] >> color[1] >> color[2] >> color[3];
    emitter.color = sf::Color(color[0], color[1], color[2], color[3]);
    in >> emitter.affectedByGravity;

    emitter.setEmitterLifeSpan(emitter.lifespan);
    emitter.setParticleLifeSpan(emitter.particleLifespan);

    return in;
}
