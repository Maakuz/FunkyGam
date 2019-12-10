#include "ParticleHandler.h"
#include <string>
#include <fstream>
#include "Game/Misc/UnorderedErase.h"
#include "Game/Misc/Definitions.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Collision/CollisionHandler.h"

std::vector<Emitter*> ParticleHandler::activeEmitters;
std::vector<Emitter*> ParticleHandler::emitterQueue;
std::vector<Emitter*> ParticleHandler::dyingEmitters;

std::vector<Emitter> ParticleHandler::emitterTemplates;
std::vector<std::string> ParticleHandler::templateNames;


ParticleHandler::ParticleHandler()
{
    ConsoleWindow::get().addCommand("reloadParticles", [&](Arguments args)->std::string 
        {
            loadEmitters();

            return "Relodd";
        });

    loadEmitters();
}

ParticleHandler::~ParticleHandler()
{
    reset();
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

    for (size_t i = 0; i < dyingEmitters.size(); i++)
    {
        if (dyingEmitters[i]->isVeryDead())
        {
            delete dyingEmitters[i];
            unordered_erase(dyingEmitters, dyingEmitters.begin() + i--);
        }

        else
        {
            dyingEmitters[i]->update(dt);
        }
    }

    for (size_t i = 0; i < emitterQueue.size(); i++)
    {
        emitterQueue[i]->update(dt);
    }
}

void ParticleHandler::queueLights()
{
    for (Emitter* emitter : this->activeEmitters)
        emitter->queueLights();

    for (Emitter* emitter : this->emitterQueue)
        emitter->queueLights();

    for (Emitter* emitter : this->dyingEmitters)
        emitter->queueLights();
}

void ParticleHandler::handleCollision()
{ 
    for (Emitter* emitter : this->activeEmitters)
        if (emitter->isColliding())
            emitter->handleCollision(CollisionHandler::getStaticColliders());

    for (Emitter* emitter : this->emitterQueue)
        if (emitter->isColliding())
            emitter->handleCollision(CollisionHandler::getStaticColliders());

    for (Emitter* emitter : this->dyingEmitters)
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
    for (Emitter* emitter : ParticleHandler::activeEmitters)
        delete emitter;

    for (Emitter* emitter : ParticleHandler::dyingEmitters)
        delete emitter;

    activeEmitters.clear();
    dyingEmitters.clear();
    emitterQueue.clear();
}

void ParticleHandler::addEmitter(int emitterID, sf::Vector2f pos)
{
    Emitter* emitter = new Emitter(emitterTemplates[emitterID]);
    emitter->setEmitterPos(pos);
    emitter->reset();
    activeEmitters.push_back(emitter);
}

Emitter* ParticleHandler::createEmitter(int emitterID, sf::Vector2f pos)
{
    Emitter* emitter = new Emitter(emitterTemplates[emitterID]);
    emitter->setEmitterPos(pos);
    emitter->reset();
    return emitter;
}

void ParticleHandler::queueEmitter(Emitter* emitter)
{
    ParticleHandler::emitterQueue.push_back(emitter);
}

void ParticleHandler::destroyEmitter(Emitter*& emitter, bool killQuick)
{
    dyingEmitters.push_back(emitter);

    emitter = nullptr;

    if (killQuick)
        dyingEmitters.back()->killQuick();

    else
        dyingEmitters.back()->kill();
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

    for (const auto& emitter : emitterQueue)
        target.draw(*emitter, states);

    for (const auto& emitter : dyingEmitters)
        target.draw(*emitter, states);
}
