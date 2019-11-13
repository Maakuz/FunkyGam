#pragma once
#include "Game/Particles/Emitter.h"
#include "SFML/Graphics/Drawable.hpp"
#include <string>

class ParticleHandler : public sf::Drawable
{
public:

    ParticleHandler();
    ~ParticleHandler();

    void update(float dt);
    void queueLights();
    void handleCollision();
    void loadEmitters();
    void reset();

    static Emitter* addEmitter(int emitterID, sf::Vector2f pos);
    static const std::vector<Emitter>* getEmitterTemplates() { return &emitterTemplates; };
    static std::string getEmitterName(int id);
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    static std::vector<Emitter*> activeEmitters;
    static std::vector<Emitter> emitterTemplates;
    static std::vector<std::string> templateNames;
};