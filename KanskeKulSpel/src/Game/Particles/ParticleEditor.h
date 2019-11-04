#pragma once
#include "Emitter.h"

class ParticleEditor
{
public:
    ParticleEditor();
    ~ParticleEditor() {};

    void openWindow();
    void closeWindow();
private:
    bool open;

    struct PlayVariables
    {
        float lifeSpan, particleLife, spawnRate, speed;
        int initailParticles, pps, angle, cone;
        int color[4];
        int colorDev[4];
        int clearColor[3];
        sf::Vector2f size;
        bool gravityOn;
        float jitter;
        float friction;
        float particleLightRadius;

        friend std::istream& operator>>(std::istream& in, PlayVariables& variables)
        {
            std::string trash;
            in >> trash >> trash;
            in >> variables.initailParticles;
            in >> variables.pps;
            in >> variables.lifeSpan;
            in >> variables.particleLife;
            in >> variables.speed;
            in >> variables.spawnRate;
            in >> variables.angle;
            in >> variables.cone;
            in >> variables.size.x >> variables.size.y;
            in >> variables.color[0] >> variables.color[1] >> variables.color[2] >> variables.color[3];
            in >> variables.colorDev[0] >> variables.colorDev[1] >> variables.colorDev[2] >> variables.colorDev[3];
            in >> variables.gravityOn;
            in >> variables.jitter;
            in >> variables.friction;
            in >> trash;
            in >> variables.particleLightRadius;
            return in;
        };
    };

    PlayVariables variables;

};