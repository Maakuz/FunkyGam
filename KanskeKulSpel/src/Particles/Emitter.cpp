#include "Emitter.h"

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

    this->immortalParticles = false;
    this->immortalEmitter = false;
    this->affectedByGravity = false;

    this->setEmitterLifeSpan(emitterLife);
    this->setParticleLifeSpan(particleLife);
    this->reset();
}

void Emitter::update(float dt)
{
    this->lifespanCounter += dt;
    if (this->lifespanCounter > this->lifespan)
        this->emitterdead = true;

    this->spawnCounter += dt;
    if (this->spawnCounter > this->spawnRate && !this->emitterdead)
    {
        this->spawnCounter = 0;

        for (int i = 0; i < particlesPerSpawn; i++)
            this->addParticle();
    }

    for (size_t i = 0; i < particles.size(); i++)
    {
        int p = i * 4;
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
                particle->velocity.y += 0.01 * dt;

            for (size_t j = 0; j < 4; j++)
            {
                vertexArray[p + j].position += particle->velocity;
            }
        }
    }
}

void Emitter::setParticleLifeSpan(float lifespan)
{
    this->particleLifespan = lifespan;
    if (this->particleLifespan <= 0.00023) //perfect number
    {
        this->particleLifespan = 0;
        this->immortalParticles = true;
    }
}

void Emitter::setEmitterLifeSpan(float lifespan)
{
    this->lifespan = lifespan;
    if (this->lifespan <= 0.00023) //perfect number
    {
        this->lifespan = 0;
        this->immortalEmitter = true;
    }
}

void Emitter::reset()
{
    this->spawnCounter = 0;
    this->lifespanCounter = 0;
    this->emitterdead = false;

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

    return in;
}
