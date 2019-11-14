#pragma once
#include "Emitter.h"

class ParticleEditor
{
public:
    ParticleEditor();
    ~ParticleEditor() {};

    ParticleEditor(const ParticleEditor&) = delete;

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

    int selectedKeyFrame;

    bool repeating;
    bool lightOn;
    float zoomLevel;

    struct PlayVariables
    {
        float lifeSpan;
        int initailParticles;
        float color[4];
        float colorDev[4];
        int clearColor[3];
        bool collisionOn;
        bool hasLight;

        friend std::istream& operator>>(std::istream& in, PlayVariables& variables)
        {
            std::string trash;
            in >> trash >> trash;
            in >> variables.initailParticles;
            in >> variables.lifeSpan;
            in >> variables.collisionOn;
            in >> variables.hasLight;
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