#pragma once
#include "Particles/Emitter.h"
#include "SFML/Graphics/Drawable.hpp"

class ParticleHandler : public sf::Drawable
{
public:

    ParticleHandler();
    ~ParticleHandler();

    void update(float dt);

    void loadEmitters();

    static Emitter* addEmitter(int emitterID, sf::Vector2f pos);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    static std::vector<Emitter*> activeEmitters;
    static std::vector<Emitter> emitterTemplates;
};