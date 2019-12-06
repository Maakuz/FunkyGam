#pragma once
#include "Game/Particles/Emitter.h"
#include "SFML/Graphics/Drawable.hpp"
#include <string>

class ParticleHandler : public sf::Drawable
{
public:

    ParticleHandler();
    virtual ~ParticleHandler();

    void update(float dt);
    void queueLights();
    void handleCollision();
    void loadEmitters();
    void reset();
    void clearEmitterQueue() { emitterQueue.clear(); };

    static void addEmitter(int emitterID, sf::Vector2f pos);
    static Emitter* createEmitter(int emitterID, sf::Vector2f pos = sf::Vector2f());
    static void queueEmitter(Emitter* emitter);

    //The particlehandler will make sure the memory is released after the emitter is killed
    static void destroyEmitter(Emitter*& emitter, bool killQuick = false);

    static const std::vector<Emitter>* getEmitterTemplates() { return &emitterTemplates; };
    static std::string getEmitterName(int id);
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    static std::vector<Emitter*> activeEmitters;
    static std::vector<Emitter*> emitterQueue;
    static std::vector<Emitter*> dyingEmitters;

    static std::vector<Emitter> emitterTemplates;
    static std::vector<std::string> templateNames;
};