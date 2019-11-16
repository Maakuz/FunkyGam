#include "Emitter.h"
#include "Renderer/Lighting/LightQueue.h"
#include "Misc/ConsoleWindow.h"

Emitter::Emitter()
{
    this->colliding = false;
    this->initialParticles = 0;
    this->particlesHasLight = false;
    this->elapsedTime = 0;
    this->prev = 0;
    this->next = 0;
    this->freezeTime = false;
    this->moved = false;

    KeyFrame frame(0);

    this->pos = pos;
    frame.speed = 1;
    frame.spawnRate = 100;
    frame.size = sf::Vector2f(1, 1);
    frame.particleLifespan = 3000;
    frame.color = sf::Color::Green;
    frame.particlesPerSpawn = 1;
    frame.emitterAngle = 0;
    frame.emitterCone = 360;
    frame.frictionValue = 1;
    frame.jitterAmount = 0;
    frame.gravity = 0;
    frame.offset = 0;

    frame.followsCenter = false;
    frame.affectedByGravity = false;
    frame.particleLightRadius = 50;

    keyFrames.push_back(frame);

    this->setEmitterLifeSpan(0);
    this->reset();
}

Emitter::Emitter(const Emitter& other)
{
    *this = other;
}

Emitter& Emitter::operator=(const Emitter& other)
{
    keyFrames = other.keyFrames;

    pos = other.pos;
    vertexArray = other.vertexArray;
    initialParticles = other.initialParticles;
    lifespan = other.lifespan;
    spawnCounter = other.spawnCounter;
    colliding = other.colliding;
    immortalEmitter = other.immortalEmitter;
    emitterDead = other.emitterDead;
    particlesHasLight = other.particlesHasLight;

    this->prev = 0;
    this->next = 0;

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
    if (!freezeTime)
        elapsedTime += dt;

    while (keyFrames[next].timeStamp < elapsedTime && prev < keyFrames.size() - 1)
    {
        prev = next;
        if (next < keyFrames.size() - 1)
            next++;
    }

    float lerp = 1;

    if (prev != next)
        lerp = (elapsedTime - keyFrames[prev].timeStamp) / (keyFrames[next].timeStamp - keyFrames[prev].timeStamp);

    lerpLights(lerp);
    int spawnRate = ((1-lerp) * keyFrames[prev].spawnRate) + (lerp * keyFrames[next].spawnRate);
    int particlesPerSpawn = ((1 - lerp) * keyFrames[prev].particlesPerSpawn) + (lerp * keyFrames[next].particlesPerSpawn);
    bool affectedByGravity = keyFrames[prev].affectedByGravity;
    bool followsCenter = keyFrames[prev].followsCenter;
    float gravity = ((1 - lerp) * keyFrames[prev].gravity) + (lerp * keyFrames[next].gravity);
    float frictionValue = ((1 - lerp) * keyFrames[prev].frictionValue) + (lerp * keyFrames[next].frictionValue);
    float jitterAmount = ((1 - lerp) * keyFrames[prev].jitterAmount) + (lerp * keyFrames[next].jitterAmount);

    if (!immortalEmitter)
    {
        if (this->elapsedTime > this->lifespan)
        {
            this->emitterDead = true;
            for (KeyFrame& frame : keyFrames)
            {
                for (EmitterLight& light : frame.lights)
                {
                    light.light->color.x *= 0.95f;
                    light.light->color.y *= 0.95f;
                    light.light->color.z *= 0.95f;
                }
            }
        }
    }
    this->spawnCounter += dt;
    if (this->spawnCounter > spawnRate && !this->emitterDead)
    {
        this->spawnCounter = 0;

        for (int i = 0; i < particlesPerSpawn; i++)
            this->addParticle(lerp);

    }

    for (size_t i = 0; i < particles.size(); i++)
    {
        int p = (int)i * 4;
        Particle* particle = particles[i];

        particle->lifespan -= dt;

        if (particlesHasLight && particle->lifespan / particle->lifespanMax < 0.1)
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
            if (affectedByGravity)
                particle->velocity.y += gravity * dt;

            particle->velocity *= frictionValue;

            if (jitterAmount > 0.00023) //perfect number
            {
                float angle = std::rand() % 360 * 3.14f / 180.f;
                particle->velocity += sf::Vector2f(std::cos(angle) * jitterAmount, std::sin(angle) * jitterAmount);
            }

            for (size_t j = 0; j < 4; j++)
            {
                if (followsCenter && moved)
                    vertexArray[p + j].position += (pos - prevPos);

                vertexArray[p + j].position += particle->velocity;
            }

            if (particlesHasLight) 
            {
                if (followsCenter && moved)
                    particles[i]->light->pos += (pos - prevPos);

                particles[i]->light->pos += particle->velocity;
            }
        }
    }

    this->moved = false;
}

void Emitter::queueLights()
{
    if (particlesHasLight)
    {
        for (Particle* particle : this->particles)
            LightQueueNoShadow::get().queue(particle->light);
    }

    std::vector<EmitterLight>* bigger;
    std::vector<EmitterLight>* smaller;
    if (keyFrames[prev].lights.size() > keyFrames[next].lights.size())
    {
        bigger = &keyFrames[prev].lights;
        smaller = &keyFrames[next].lights;
    }

    else
    {
        bigger = &keyFrames[next].lights;
        smaller = &keyFrames[prev].lights;
    }

    for (int i = 0; i < bigger->size(); i++)
    {
        if (i < smaller->size())
        {
            if (bigger->at(i).initialRadius > smaller->at(i).initialRadius)
                LightQueue::get().queue(bigger->at(i).light);

            else
                LightQueue::get().queue(smaller->at(i).light);
        }

        else
            LightQueue::get().queue(bigger->at(i).light);
    }
}

void Emitter::lerpLights(float lerp)
{
    if (prev == next)
        return;

    std::vector<EmitterLight>* biggerVec;
    std::vector<EmitterLight>* smallerVec;
    if (keyFrames[prev].lights.size() > keyFrames[next].lights.size())
    {
        biggerVec = &keyFrames[prev].lights;
        smallerVec = &keyFrames[next].lights;
    }

    else
    {
        biggerVec = &keyFrames[next].lights;
        smallerVec = &keyFrames[prev].lights;
    }

    for (int i = 0; i < biggerVec->size(); i++)
    {
        if (i < smallerVec->size())
        {
            std::vector<EmitterLight>* finalVec;
            if (biggerVec->at(i).initialRadius > smallerVec->at(i).initialRadius)
                finalVec = biggerVec;

            else
                finalVec = smallerVec;

            finalVec->at(i).offset = ((1 - lerp) * keyFrames[prev].lights[i].initialOffset)
                + (lerp * keyFrames[next].lights[i].initialOffset);

            finalVec->at(i).light->pos = this->pos + finalVec->at(i).offset;

            finalVec->at(i).light->color = ((1 - lerp) * keyFrames[prev].lights[i].initialColor)
                + (lerp * keyFrames[next].lights[i].initialColor);

            finalVec->at(i).light->radius = ((1 - lerp) * keyFrames[prev].lights[i].initialRadius)
                    + (lerp * keyFrames[next].lights[i].initialRadius);
                
            
        }
    }
}

void Emitter::handleCollision(const std::vector<Entity*>* entities)
{
    for (const Entity* entity : *entities)
    {
        const Collider collider = entity->getCollider();
        if (collider.hasComponent(Collider::ColliderKeys::ground))
        {
            for (int i = 0; i < particles.size(); i++)
            {
                sf::Vector2f pos = vertexArray[i * 4].position;

                if (collider.contains(pos))
                {
                    if (Collider::contains(collider.getLeft(), pos) || Collider::contains(collider.getRight(), pos))
                        particles[i]->velocity.x *= -0.5f;

                    if (Collider::contains(collider.getDown(), pos) || Collider::contains(collider.getUp(), pos))
                        particles[i]->velocity.y *= -0.5f;
                }
            }
        }
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

void Emitter::setElapsedTime(float time)
{
    prev = 0;
    next = 0;
    this->elapsedTime = time;
    for (int i = 0; i < keyFrames.size() && next == 0; i++)
    {
        if (keyFrames[i].timeStamp < elapsedTime)
            prev = i;

        else
            next = i;
    }
}

void Emitter::addKeyFrame(int pos)
{
    KeyFrame next = keyFrames[pos - 1];
    next.timeStamp++;
    keyFrames.insert(keyFrames.begin() + pos, next); 
}

void Emitter::kill()
{
    this->immortalEmitter = false;
    this->elapsedTime = this->lifespan + 1;
}

void Emitter::killQuick()
{
    this->kill();

    for (Particle* particle : this->particles)
        particle->lifespan = std::min(particle->lifespan, 100.f);
}

//USE RESPONSIBLY
void Emitter::setParticleHasLight(bool hasLight)
{ 
    this->particlesHasLight = hasLight; 
    reset();
}

void Emitter::setEmitterPos(sf::Vector2f pos)
{ 
    this->moved = true;
    this->prevPos = this->pos;
    this->pos = pos; 
    for (KeyFrame& frame : keyFrames)
        for (EmitterLight& light : frame.lights)
            light.light->pos = pos + light.offset;
}

bool Emitter::isVeryDead() const
{
    return this->emitterDead && particles.empty();
}

void Emitter::reset()
{
    this->spawnCounter = 0;
    this->emitterDead = false;
    this->elapsedTime = 0;
    this->next = 0; //if this does not exist we have problems'
    this->prev = 0;

    for (Particle* particle : particles)
        delete particle;

    this->particles.clear();
    this->vertexArray.clear();
    


    for (int i = 0; i < initialParticles; i++)
    {
        this->addParticle(0);
    }

    for (KeyFrame& frame : keyFrames)
        for (EmitterLight& light : frame.lights)
            light.light->color = light.initialColor;
}

void Emitter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!vertexArray.empty())
        target.draw(&vertexArray[0], vertexArray.size(), sf::Quads);
}

void Emitter::addParticle(float lerp)
{
    int emitterCone = ((1 - lerp) * keyFrames[prev].emitterCone) + (lerp * keyFrames[next].emitterCone);
    int emitterAngle = ((1 - lerp) * keyFrames[prev].emitterAngle) + (lerp * keyFrames[next].emitterAngle);
    int particleLightRadius = ((1 - lerp) * keyFrames[prev].particleLightRadius) + (lerp * keyFrames[next].particleLightRadius);
    float particleLifespan = ((1 - lerp) * keyFrames[prev].particleLifespan) + (lerp * keyFrames[next].particleLifespan);
    float speed = ((1 - lerp) * keyFrames[prev].speed) + (lerp * keyFrames[next].speed);
    float offset = ((1 - lerp) * keyFrames[prev].offset) + (lerp * keyFrames[next].offset);

    sf::Color colorDeviation;
    colorDeviation.r = ((1 - lerp) * keyFrames[prev].colorDeviation.r) + (lerp * keyFrames[next].colorDeviation.r);
    colorDeviation.g = ((1 - lerp) * keyFrames[prev].colorDeviation.g) + (lerp * keyFrames[next].colorDeviation.g);
    colorDeviation.b = ((1 - lerp) * keyFrames[prev].colorDeviation.b) + (lerp * keyFrames[next].colorDeviation.b);
    colorDeviation.a = ((1 - lerp) * keyFrames[prev].colorDeviation.a) + (lerp * keyFrames[next].colorDeviation.a);
    
    sf::Color color;
    color.r = ((1 - lerp) * keyFrames[prev].color.r) + (lerp * keyFrames[next].color.r);
    color.g = ((1 - lerp) * keyFrames[prev].color.g) + (lerp * keyFrames[next].color.g);
    color.b = ((1 - lerp) * keyFrames[prev].color.b) + (lerp * keyFrames[next].color.b);
    color.a = ((1 - lerp) * keyFrames[prev].color.a) + (lerp * keyFrames[next].color.a);

    sf::Vector2f size = ((1 - lerp) * keyFrames[prev].size) + (lerp * keyFrames[next].size);

    float angle = ((std::rand() % emitterCone) + emitterAngle) * 3.14f / 180.f;
    float randSpeed = ((std::rand() % 25) / 100.f) + speed;
    sf::Vector2f dir(std::cos(angle), std::sin(angle));
    sf::Vector2f startPos = this->pos + (dir * offset);
    Particle* particle = new Particle(randSpeed * dir, color, particleLifespan);

    if (colorDeviation.r != 0)
        particle->color.r += std::rand() % (int)colorDeviation.r;
    if (colorDeviation.g != 0)
        particle->color.g += std::rand() % (int)colorDeviation.g;
    if (colorDeviation.b != 0)
        particle->color.b += std::rand() % (int)colorDeviation.b;

    //oh bobby i'm scared
    if (particlesHasLight)
        particle->light = new Light_NoShadow(startPos, particleLightRadius, sf::Vector3f(particle->color.r / 255.f, particle->color.g / 255.f, particle->color.b / 255.f));

    particles.push_back(particle);

    vertexArray.push_back(sf::Vertex(startPos + sf::Vector2f(-size.x, -size.y), particle->color));
    vertexArray.push_back(sf::Vertex(startPos + sf::Vector2f(size.x, -size.y), particle->color));
    vertexArray.push_back(sf::Vertex(startPos + sf::Vector2f(size.x, size.y), particle->color));
    vertexArray.push_back(sf::Vertex(startPos + sf::Vector2f(-size.x, size.y), particle->color));

}

std::ostream& operator<<(std::ostream& out, const Emitter& emitter)
{
    out << emitter.pos.x << " " << emitter.pos.y << "\n";
    out << emitter.initialParticles << "\n";
    out << emitter.lifespan << "\n";
    out << emitter.colliding << "\n";
    out << emitter.particlesHasLight << "\n";

    out << emitter.keyFrames.size() << "\n";
    for (int i = 0; i < emitter.keyFrames.size(); i++)
    {
        out << emitter.keyFrames[i] << "\n";
    }


    return out;
}

std::istream& operator>>(std::istream& in, Emitter& emitter)
{
    in >> emitter.pos.x >> emitter.pos.y;
    in >> emitter.initialParticles;
    in >> emitter.lifespan;
    in >> emitter.colliding;
    in >> emitter.particlesHasLight;

    int keyFrameCount = 0;
    in >> keyFrameCount;

    emitter.keyFrames.clear();
    for (int i = 0; i < keyFrameCount; i++)
    {
        Emitter::KeyFrame frame;
        in >> frame;
        emitter.keyFrames.push_back(frame);
    }

    emitter.prev = 0;
    emitter.next = 0;

    emitter.setEmitterLifeSpan(emitter.lifespan);

    return in;
}
