#include "ParticleHandler.h"
#include <string>
#include <fstream>
#include "Game/Misc/UnorderedErase.h"
#include "Game/Misc/Definitions.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Collision/CollisionHandler.h"

std::vector<Emitter*> ParticleHandler::activeEmitters;
std::vector<Emitter> ParticleHandler::emitterTemplates;
std::vector<std::string> ParticleHandler::templateNames;


ParticleHandler::ParticleHandler()
{
    ConsoleWindow::get().addCommand("reloadParticles", [&](Arguments args)->std::string 
        {
            loadEmitters();

            return "Relodd";
        });
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

void ParticleHandler::queueLights()
{
    for (Emitter* emitter : this->activeEmitters)
        emitter->queueLights();
}

void ParticleHandler::handleCollision()
{ 
    for (Emitter* emitter : this->activeEmitters)
        if (emitter->isColliding())
            emitter->handleCollision(CollisionHandler::getStaticColliders());

}

void ParticleHandler::loadEmitters()
{
    emitterTemplates.clear();
    templateNames.clear();

    std::ifstream loadlist(DATA_PATH "Particles.mop");

    if (!loadlist.is_open())
    {
        system("Pause");
        exit(-49);
    }

    std::string trash;
    int particleCount;
    std::string folder;


    loadlist >> trash >> folder;

    while (!loadlist.eof())
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
            templateNames.push_back(fileName);
        }

        else
            printf("Particle %s could not be loaded.\n", fileName.c_str());
    }

    loadlist.close();
}

void ParticleHandler::reset()
{
    for (auto& emitter : ParticleHandler::activeEmitters)
        delete emitter;

    activeEmitters.clear();
}

Emitter* ParticleHandler::addEmitter(int emitterID, sf::Vector2f pos)
{
    Emitter* emitter = new Emitter(emitterTemplates[emitterID]);
    emitter->setEmitterPos(pos);
    emitter->reset();
    activeEmitters.push_back(emitter);

    return emitter;
}

std::string ParticleHandler::getEmitterName(int id) 
{
    if (id < 0)
        return "None";

    return templateNames[id]; 
}

void ParticleHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const auto& emitter : activeEmitters)
        target.draw(*emitter, states);
}
