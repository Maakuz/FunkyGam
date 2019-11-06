#include "ParticleEditor.h"
#include "Game/Misc/Definitions.h"
#include "Imgui/imgui.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"
#include <filesystem>
#include <fstream>
#include "Renderer/Renderer.h"

namespace fs = std::filesystem;

ParticleEditor::ParticleEditor()
{
    open = false;
    gridColor = sf::Color::Magenta;
    grid = generateGrid(gridColor);
    repeating = false;
    lightOn = false;
    zoomLevel = 1;
    color = sf::Color(variables.color[0], variables.color[1], variables.color[2], variables.color[3]);
    this->currentEmitter = -1;
}

void ParticleEditor::update(sf::Vector2f mousePosWorld, float dt)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        emitto.setEmitterPos(mousePosWorld);
    
    //imgui block
    {
        ImGui::Begin("Pickle a Particle", &this->open);
        ImGui::BeginTabBar("Lekrutan");
        if (ImGui::BeginTabItem("Particles"))
        {
            ImGui::DragFloat("Particle lifespan", &variables.particleLife, 1, 0, INT_MAX);
            ImGui::DragFloat("Emitter lifespan", &variables.lifeSpan, 1, 0, INT_MAX);
            ImGui::DragFloat("Particle speed", &variables.speed, 0.01f, 0, INT_MAX);
            ImGui::DragFloat("Spawn rate", &variables.spawnRate, 1, 0, INT_MAX);
            ImGui::DragInt("Initial particles", &variables.initailParticles, 1, 0, INT_MAX);
            ImGui::DragInt("Particle per spawn", &variables.pps, 1, 1, INT_MAX);
            ImGui::DragFloat("Size x", &variables.size.x, 0.05f, 0, INT_MAX);
            ImGui::DragFloat("Size y", &variables.size.y, 0.05f, 0, INT_MAX);
            ImGui::DragInt("Angle", &variables.angle, 1, 0, 360);
            ImGui::DragInt("Cone", &variables.cone, 1, 1, 360);
            ImGui::DragInt4("Color", variables.color, 1, 0, 255);
            ImGui::DragInt4("Color Deviation", variables.colorDev, 1, 0, 255);
            ImGui::DragFloat("Friction", &variables.friction, 0.001, 0, 2);
            ImGui::DragFloat("Jitter", &variables.jitter, 0.001, 0, 1);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Lighting"))
        {
            if (ImGui::Button("Add light"))
            {
                emitto.addLight(sf::Vector2f(0, 0), 100, sf::Vector3f(1, 1, 1));
            }

            ImGui::DragFloat("Particle light radius", &variables.particleLightRadius, 1, 0, 200);
            ImGui::Separator();
            ImGui::BeginChild("LightList", sf::Vector2i(0, -200));

            for (size_t i = 0; i < emitto.getLights()->size(); i++)
            {
                std::string collabel = "color " + std::to_string(i);
                std::string poslabel = "pos " + std::to_string(i);
                std::string radiuslabel = "radius " + std::to_string(i);

                Emitter::EmitterLight* light = &emitto.getLights()->at(i);

                float col[3] = { light->initialColor.x, light->initialColor.y, light->initialColor.z };
                float pos[2] = { light->offset.x, light->offset.y };
                float radius = light->light->radius;

                //Gotta make a new light if radius is altered. Thanks shadows
                if (ImGui::DragFloat(radiuslabel.c_str(), &radius, 1, 10, 3000))
                {
                    delete light->light;
                    light->light = new Light(light->offset, radius, light->initialColor);
                }

                if (ImGui::ColorEdit3(collabel.c_str(), col))
                {
                    light->light->color.x = col[0];
                    light->light->color.y = col[1];
                    light->light->color.z = col[2];
                    light->initialColor.x = col[0];
                    light->initialColor.y = col[1];
                    light->initialColor.z = col[2];
                }

                ImGui::DragFloat2(poslabel.c_str(), pos, 1, -1000, 1000);
                ImGui::Separator();


                light->offset.x = pos[0];
                light->offset.y = pos[1];
                light->light->pos = emitto.getEmitterPos() + light->offset;
            }


            ImGui::EndChild();
            ImGui::Separator();


            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Editor"))
        {
            float col[3] = { gridColor.r, gridColor.g, gridColor.b };

            if (ImGui::DragFloat3("grid color", col, 1, 0, 255))
            {
                gridColor = sf::Color(col[0], col[1], col[2]);
                grid = generateGrid(gridColor);
            }

            ImGui::DragFloat("Zooooom", &zoomLevel, 1, 1, 8);

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();

        ImGui::DragInt3("Background color", variables.clearColor, 1, 0, 255);
        ImGui::Checkbox("Enable gravity", &variables.gravityOn);
        ImGui::SameLine();
        ImGui::Checkbox("Enable light", &lightOn);
        ImGui::SameLine();
        ImGui::Checkbox("Repeating", &repeating);
        ImGui::SameLine();
        if (ImGui::Checkbox("Enable particle lights!?", &variables.hasLight))
            emitto.setParticleHasLight(variables.hasLight);

        ImGui::Text("Emitter: %s,", fileNames[currentEmitter].c_str());
        ImGui::SameLine();
        ImGui::Text("Particles: %d,", emitto.getNrOfParticles());
        ImGui::SameLine();
        ImGui::Text("Microseconds to update: %d", particleUpdateTime.asMicroseconds());

        if (ImGui::Button("Reset!"))
            emitto.reset();

        ImGui::SameLine();
        static bool saveWindowOpen = false;
        if (ImGui::Button("Save!"))
            saveWindowOpen = true;

        ImGui::SameLine();
        static bool loadWindowOpen = false;
        if (ImGui::Button("Load!"))
            loadWindowOpen = true;

        ImGui::SameLine();
        if (ImGui::Button("New!"))
        {
            restart(&emitto);
        }

        ImGui::SameLine();
        static bool deleteWindowOpen = false;
        if (ImGui::Button("Delete items!"))
            deleteWindowOpen = true;


        if (saveWindowOpen)
        {
            static std::string helperText;
            if (emitterName.empty())
                helperText = "Enter filename";

            for (std::string name : fileNames)
                if ((emitterName + ".part") == name)
                    helperText = "Filename exist and will be overwritten"
                                 "if not changed IN FIVE SECONDS!";
            
            ImGui::Begin("Save me please!", &saveWindowOpen);

            ImGui::Text(helperText.c_str());
            ImGui::InputText("Filename", &emitterName);

            if (ImGui::Button("Save!!"))
            {
                if (emitterName.size() > 0)
                {
                    save();
                    helperText = "File saved hopefully!";
                }
            }
            ImGui::End();
        }

        if (loadWindowOpen)
        {
            ImGui::Begin("Load me please!", &loadWindowOpen);
            if (!fileNames.empty())
            {
                if (ImGui::BeginCombo("Particles", fileNames[currentEmitter].c_str())) //Hm().MM
                {
                    for (int i = 0; i < fileNames.size(); i++)
                    {

                        if (ImGui::Selectable(fileNames[i].c_str()))
                            load(i);

                    }

                    ImGui::EndCombo();
                }
            }


            ImGui::End();
        }

        if (deleteWindowOpen)
        {
            ImGui::Begin("Erase me please no!", &deleteWindowOpen);
            if (!fileNames.empty())
            {
                static int selectedItem = 0;
                if (ImGui::BeginCombo("Particles", fileNames[selectedItem].c_str())) //Hm().MM
                {
                    for (int i = 0; i < fileNames.size(); i++)
                    {

                        if (ImGui::Selectable(fileNames[i].c_str()))
                            selectedItem = i;

                    }

                    ImGui::EndCombo();
                }

                if (ImGui::Button("Delete!"))
                {
                    fs::remove(fs::path(emitterFolder + fileNames[selectedItem]));
                    fileNames.erase(fileNames.begin() + selectedItem);
                    if (currentEmitter == selectedItem)
                        currentEmitter = fileNames.size() -1;
                    selectedItem = fileNames.size() - 1;
                    saveParticleList();
                }

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("THIS IS THE FINAL WARNING. THE OBJECT WILL\nDIE IF YOU CLICK HERE :(");
            }


            ImGui::End();
        }
        ImGui::End();
    }

    if (repeating)
        if (emitto.isVeryDead())
            emitto.reset();

    emitto.setParticleLifeSpan(variables.particleLife);
    emitto.setEmitterLifeSpan(variables.lifeSpan);
    emitto.setSpeed(variables.speed);
    emitto.setSpawnRate(variables.spawnRate);
    emitto.setInitialParticles(variables.initailParticles);
    emitto.setParticlesPerSpawn(variables.pps);
    emitto.setSize(variables.size);
    emitto.enableGravity(variables.gravityOn);
    emitto.setAngle(variables.angle);
    emitto.setConeSize(variables.cone);
    emitto.setFriction(variables.friction);
    emitto.setJitter(variables.jitter);
    emitto.setParticleLightRadius(variables.particleLightRadius);


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
        emitto.kill();

    updateTimer.restart();
    emitto.update(dt);
    particleUpdateTime = updateTimer.getElapsedTime();

    color.r = variables.color[0];
    color.g = variables.color[1];
    color.b = variables.color[2];
    color.a = variables.color[3];
    emitto.setColor(color);

    color.r = variables.colorDev[0];
    color.g = variables.colorDev[1];
    color.b = variables.colorDev[2];
    color.a = variables.colorDev[3];
    emitto.setColorDeviation(color);

    emitto.queueLights();
    Renderer::queueDrawable(&emitto);
}

void ParticleEditor::openWindow()
{
    this->open = true;
    this->getParticleList();
    if (currentEmitter != -1)
        load(currentEmitter);
}

void ParticleEditor::closeWindow()
{
    this->open = false;
}

void ParticleEditor::save()
{
    emitterName += ".part";
    std::ofstream file(emitterFolder + emitterName);

    if (!file.is_open())
    {
        printf("oof\n");
    }

    else
    {
        file << emitto;
        file.close();

        bool exists = false;
        for (std::string str : fileNames)
            if (str == emitterName)
                exists = true;

        if (!exists)
        {
            fileNames.push_back(emitterName);
            this->currentEmitter = fileNames.size() - 1;
            saveParticleList();
        }
    }
}

void ParticleEditor::load(int id)
{
    std::ifstream file(emitterFolder + fileNames[id]);

    if (!file.is_open())
    {
        printf("oof\n");
    }

    else
    {
        file >> emitto;
        file.clear();
        file.seekg(0, std::ios::beg);

        file >> variables;

        file.close();
        emitto.reset();
        this->currentEmitter = id;
    }
}

void ParticleEditor::getParticleList()
{
    fileNames.clear();
    std::ifstream file(DATA_PATH "Particles.mop");

    if (!file.is_open())
    {
        system("Pause");
        exit(-49);
    }

    std::string trash;

    file >> trash >> this->emitterFolder;


    while (!file.eof())
    {
        std::string fileName;
        file >> trash >> fileName;
        fileNames.push_back(fileName);
    }
    file.close();

    
    if (!fileNames.empty())
        currentEmitter = fileNames.size() - 1;

}

void ParticleEditor::saveParticleList()
{
    std::ofstream file(DATA_PATH "Particles.mop");

    if (!file.is_open())
    {
        printf("oof\n");
    }

    else
    {
        file << "Folder: " << emitterFolder;

        for (int i = 0; i < fileNames.size(); i++)
        {
            file << "\n" << i << ": " << fileNames[i];
        }
        file.close();
    }
}

sf::VertexArray ParticleEditor::generateGrid(sf::Color color, float size)
{
    sf::VertexArray grid(sf::PrimitiveType::Lines);

    for (int i = 0; i < WIN_WIDTH / size; i++)
    {
        sf::Vertex v(sf::Vector2f(i * size, 0), color);
        grid.append(v);
        v.position.y = WIN_HEIGHT;
        grid.append(v);
    }

    for (int i = 0; i < WIN_HEIGHT / size; i++)
    {
        sf::Vertex v(sf::Vector2f(0, i * size), color);
        grid.append(v);
        v.position.x = WIN_WIDTH;
        grid.append(v);
    }

    return grid;
}

void ParticleEditor::restart(Emitter* emitter)
{
    variables = { 30000, 3000, 50, 2, 500, 5, 0, 360,
        sf::Color::Green.r, sf::Color::Green.g, sf::Color::Green.b, sf::Color::Green.a,
        0, 0, 0, 0,
        0, 0, 0, sf::Vector2f(2, 5), false, 0, 1, 50 };

    sf::Color color(variables.color[0], variables.color[1], variables.color[2], variables.color[3]);


    *emitter = Emitter(sf::Vector2f(500, 500), variables.size, color, variables.spawnRate, variables.speed, variables.particleLife, variables.lifeSpan, variables.initailParticles, variables.pps);

}