#include "ParticleHandler.h"
#include <string>
#include <fstream>
#include "Misc/UnorderedErase.h"
#include "Misc/Definitions.h"

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
    std::ifstream loadlist(DATA_PATH "LoadList.mop");

    if (!loadlist.is_open())
    {
        system("Pause");
        exit(-49);
    }

    std::string trash;
    int particleCount;
    std::string folder;

    while (trash != "[Particles]")
        loadlist >> trash;

    loadlist >> trash >> folder;
    loadlist >> trash >> particleCount;


    for (int i = 0; i < particleCount; i++)
    {
        std::string fileName;
        loadlist >> trash >> fileName;

        std::ifstream file(folder + fileName);
        if (file.is_open())
        {
            Emitter emitter;
            file >> emitter;

            file.close();

            emitterTemplates.push_back(emitter);
        }

        else
            printf("Particle with ID: %d could not be loaded.\n", i);
    }

    loadlist.close();
}

Emitter const * ParticleHandler::addEmitter(int emitterID, sf::Vector2f pos)
{
    Emitter* emitter = new Emitter(emitterTemplates[emitterID]);
    emitter->setEmitterPos(pos);
    emitter->reset();
    activeEmitters.push_back(emitter);

    return emitter;
}

void ParticleHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const auto& emitter : activeEmitters)
        target.draw(*emitter, states);
}
