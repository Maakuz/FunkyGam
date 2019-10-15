#include "ParticleHandler.h"
#include <string>
#include <fstream>
#include "Misc/UnorderedErase.h"

#define PARTICLE_FOLDER "../Particles/"
#define PARTICLE_PATH(x) PARTICLE_FOLDER x

const int NR_OF_PARTICLES = 1;
const std::string PATHS[NR_OF_PARTICLES] = {PARTICLE_PATH("bombDyr.part")};

std::vector<Emitter*> ParticleHandler::activeEmitters;
std::vector<Emitter> ParticleHandler::emitterTemplates;


ParticleHandler::ParticleHandler()
{
}

ParticleHandler::~ParticleHandler()
{
    for (auto& emitter : ParticleHandler::activeEmitters)
        delete emitter;
}

void ParticleHandler::update(float dt)
{
    for (size_t i = 0; i < activeEmitters.size(); i++)
    {
        if (activeEmitters[i]->isVeryDead())
        {
            delete activeEmitters[i];
            unordered_erase(activeEmitters, activeEmitters.begin() + i--);
        }

        else
        {
            activeEmitters[i]->update(dt);
        }
    }
}

void ParticleHandler::loadEmitters()
{
    for (int i = 0; i < NR_OF_PARTICLES; i++)
    {
        std::ifstream file(PATHS[i]);
        if (file.is_open())
        {
            Emitter emitter;
            file >> emitter;

            file.close();

            emitterTemplates.push_back(emitter);
        }
    }
}

void ParticleHandler::addEmitter(emitterTypes type, sf::Vector2f pos)
{
    Emitter* emitter = new Emitter(emitterTemplates[type]);
    emitter->setEmitterPos(pos);
    activeEmitters.push_back(emitter);
}

void ParticleHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const auto& emitter : activeEmitters)
        target.draw(*emitter, states);
}
