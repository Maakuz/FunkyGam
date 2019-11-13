#pragma once
#include "Emitter.h"

class ParticleEditor
{
public:
    ParticleEditor();
    ~ParticleEditor() {};

    void update(sf::Vector2f mousePosWorld, float dt);
    void openWindow();
    void closeWindow();
    bool isOpen()const { return open; };
private:
    bool open;
    sf::VertexArray grid;
    sf::Color gridColor;
    sf::Clock updateTimer;
    sf::Time particleUpdateTime;

    sf::Color color;

    std::string emitterName;
    std::string emitterFolder;
    std::vector<std::string> fileNames;
    int currentEmitter;
    Emitter emitto;

    bool repeating;
    bool lightOn;
    float zoomLevel;

    struct PlayVariables
    {
        float lifeSpan, particleLife, spawnRate, speed;
        int initailParticles, pps, angle, cone;
        int color[4];
        int colorDev[4];
        int clearColor[3];
        sf::Vector2f size;
        bool gravityOn;
        float gravity;
        bool collisionOn;
        float jitter;
        float friction;
        float particleLightRadius;
        bool hasLight;

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
            in >> variables.gravity;
            in >> variables.collisionOn;
            in >> variables.jitter;
            in >> variables.friction;
            in >> variables.hasLight;
            in >> variables.particleLightRadius;
            return in;
        };
    };

    PlayVariables variables;

    void save();
    void saveAll();
    void load(int id);
    void getParticleList();
    void saveParticleList();
    sf::VertexArray generateGrid(sf::Color color, float size = 64);
    void restart(Emitter* emitter);
};