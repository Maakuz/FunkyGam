#pragma once
#include "Particles/Emitter.h"
#include "SFML/Graphics/Drawable.hpp"

class ParticleHandler : public sf::Drawable
{
public:
    enum emitterTypes 
    {
        bomb = 0,
        fire = 1,
        flash = 2,
        flare = 3
    };

    ParticleHandler();
    ~ParticleHandler();

    void update(float dt);

    void loadEmitters();

    static void addEmitter(emitterTypes type, sf::Vector2f pos);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    static std::vector<Emitter*> activeEmitters;
    static std::vector<Emitter> emitterTemplates;
};