#include "Emitter.h"
#include "Renderer/Lighting/LightQueue.h"

Emitter::Emitter(sf::Vector2f pos, sf::Vector2f particleSize, sf::Color color, 
    float spawnRate, float particleSpeed, float particleLife, float emitterLife, 
    int initialParticles, int particlesPerSpawn, int startAngle, int spread, 
    float frictionValue, float jitterAmount)
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
    this->frictionValue = frictionValue;
    this->jitterAmount = jitterAmount;

    this->affectedByGravity = false;
    this->particlesHasLight = false; // might be dangerous for framerate to turn on
    this->particleLightRadius = 50;

    this->setEmitterLifeSpan(emitterLife);
    this->setParticleLifeSpan(particleLife);
    this->reset();
}

Emitter::Emitter(const Emitter& other)
{
    *this = other;
}

Emitter& Emitter::operator=(const Emitter& other)
{
    lights = other.lights;

    pos = other.pos;

    vertexArray = other.vertexArray;

    initialParticles = other.initialParticles;
    particlesPerSpawn = other.particlesPerSpawn;

   lifespan = other.lifespan;
   lifespanCounter = other.lifespanCounter;

   particleLifespan = other.particleLifespan;
   speed = other.speed;

   spawnRate = other.spawnRate;
   spawnCounter = other.spawnCounter;

    emitterAngle = other.emitterAngle;
    emitterCone = other.emitterCone;

    size = other.size;
    color = other.color;
    colorDeviation = other.colorDeviation;

    affectedByGravity = other.affectedByGravity;

    immortalEmitter = other.immortalEmitter;

    emitterDead = other.emitterDead;

    particlesHasLight = other.particlesHasLight; //HIGHLY EXPERIMENTAL MAYBE LEAVE THIS AT FALSE FOREVER
    particleLightRadius = other.particleLightRadius;


    jitterAmount = other.jitterAmount;
    frictionValue = other.frictionValue;

    //Hopefully never any particles here during copying
    for (Particle* particle : this->particles)
        delete particle;
    this->particles.clear();

    for (Particle* particle : other.particles)
        this->particles.push_back(new Particle(*particle));

    return *this;
}

Emitter::~Emitter() 
{
    for (Particle* particle : particles)
        delete particle;
}

void Emitter::update(float dt)
{
    if (!immortalEmitter)
    {
        this->lifespanCounter += dt;
        if (this->lifespanCounter > this->lifespan)
        {
            this->emitterDead = true;
            for (EmitterLight& light : lights)
            {
                light.light->color.x *= 0.95f;
                light.light->color.y *= 0.95f;
                light.light->color.z *= 0.95f;
            }
        }
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
        Particle* particle = particles[i];

        particle->lifespan -= dt;

        if (particlesHasLight && particle->lifespan / particleLifespan < 0.1)
        {
            particle->light->color.x *= 0.90f;
            particle->light->color.y *= 0.90f;
            particle->light->color.z *= 0.90f;
        }

        //remove
        if (particle->lifespan < 0)
        {
            std::swap(particles[i], particles[particles.size() - 1]);
            delete particles[particles.size() - 1];
            particles.pop_back();

            for (int j = 0; j < 4; j++)
            {
                std::swap(vertexArray[p + 3 - j], vertexArray[vertexArray.size() - 1]);
                vertexArray.pop_back();
            }                
            i--;
        }

        //Moving particles
        else
        {
            if (this->affectedByGravity)
                particle->velocity.y += 0.01f * dt;

            particle->velocity *= frictionValue;

            if (jitterAmount > 0.00023) //perfect number
            {
                float angle = std::rand() % 360 * 3.14f / 180.f;
                particle->velocity += sf::Vector2f(std::cos(angle) * jitterAmount, std::sin(angle) * jitterAmount);
            }

            for (size_t j = 0; j < 4; j++)
            {
                vertexArray[p + j].position += particle->velocity;
            }

            if (particlesHasLight) //plz dont kill me frames
            {
                particles[i]->light->pos += particle->velocity;
            }
        }
    }
}

void Emitter::queueLights()
{
    if (particlesHasLight)
    {
        for (Particle* particle : this->particles)
            LightQueueNoShadow::get().queue(particle->light);
    }

    for (EmitterLight& light : lights)
        LightQueue::get().queue(light.light);
}

void Emitter::setParticleLifeSpan(float lifespan)
{
    this->particleLifespan = lifespan;
    if (this->particleLifespan <= 0.00023) //perfect number
    {
        this->particleLifespan = 0;
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

void Emitter::addLight(sf::Vector2f offset, float radius, sf::Vector3f color)
{
    EmitterLight light(this->pos, radius, color, offset);
    lights.push_back(light);
}


void Emitter::kill()
{
    this->immortalEmitter = false;
    this->lifespanCounter = this->lifespan + 1;
}

//USE RESPONSIBLY
void Emitter::setParticleHasLight(bool hasLight)
{ 
    this->particlesHasLight = hasLight; 
    reset();
}

void Emitter::setEmitterPos(sf::Vector2f pos)
{ 
    this->pos = pos; 
    for (EmitterLight& light : lights)
        light.light->pos = pos + light.offset;
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

    for (Particle* particle : particles)
        delete particle;

    this->particles.clear();
    this->vertexArray.clear();
    


    for (int i = 0; i < initialParticles; i++)
    {
        this->addParticle();
    }

    for (EmitterLight& light : lights)
    {
        light.light->color = light.initialColor;
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
    Particle* particle = new Particle(sf::Vector2f(randSpeed * std::cos(angle), randSpeed * std::sin(angle)), this->color, this->particleLifespan);
    if (colorDeviation.r != 0)
        particle->color.r += std::rand() % (int)colorDeviation.r;
    if (colorDeviation.g != 0)
        particle->color.g += std::rand() % (colorDeviation.g * 2) - colorDeviation.g;
    if (colorDeviation.b != 0)
        particle->color.b += std::rand() % (colorDeviation.b * 2) - colorDeviation.b;

    //oh bobby i'm scared
    if (particlesHasLight)
        particle->light = new Light_NoShadow(this->pos, particleLightRadius, sf::Vector3f(color.r / 255.f, color.g / 255.f, color.b / 255.f));

    particles.push_back(particle);

    vertexArray.push_back(sf::Vertex(this->pos + sf::Vector2f(-size.x, -size.y), particle->color));
    vertexArray.push_back(sf::Vertex(this->pos + sf::Vector2f(size.x, -size.y), particle->color));
    vertexArray.push_back(sf::Vertex(this->pos + sf::Vector2f(size.x, size.y), particle->color));
    vertexArray.push_back(sf::Vertex(this->pos + sf::Vector2f(-size.x, size.y), particle->color));

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
    out << (int)emitter.colorDeviation.r << " " << (int)emitter.colorDeviation.g << " " << (int)emitter.colorDeviation.b << " " << (int)emitter.color.a << "\n";
    out << emitter.affectedByGravity << "\n";
    out << emitter.jitterAmount << "\n";
    out << emitter.frictionValue << "\n";
    out << emitter.particlesHasLight << "\n";
    out << emitter.particleLightRadius << "\n";

    out << emitter.lights.size() << "\n";

    for (size_t i = 0; i < emitter.lights.size(); i++)
    {
        out << emitter.lights[i].offset.x << " " << emitter.lights[i].offset.y << "\n";
        out << emitter.lights[i].light->radius << "\n";
        out << emitter.lights[i].initialColor.x << " " << emitter.lights[i].initialColor.y << " " << emitter.lights[i].initialColor.z << "\n";
    }

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
    in >> color[0] >> color[1] >> color[2] >> color[3];
    emitter.colorDeviation = sf::Color(color[0], color[1], color[2], color[3]);
    in >> emitter.affectedByGravity;
    in >> emitter.jitterAmount;
    in >> emitter.frictionValue;
    in >> emitter.particlesHasLight;
    in >> emitter.particleLightRadius;

    int size = 0;
    in >> size;

    emitter.lights.clear();
    for (int i = 0; i < size; i++)
    {
        sf::Vector2f offset;
        float radius;
        sf::Vector3f color;

        in >> offset.x >> offset.y;
        in >> radius;
        in >> color.x >> color.y >> color.z;

        emitter.lights.push_back(Emitter::EmitterLight(emitter.pos, radius, color, offset));
    }

    emitter.setEmitterLifeSpan(emitter.lifespan);
    emitter.setParticleLifeSpan(emitter.particleLifespan);

    return in;
}
