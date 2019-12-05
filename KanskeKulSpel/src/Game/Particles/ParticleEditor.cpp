#include "ParticleEditor.h"
#include "Game/Misc/Definitions.h"
#include "Imgui/imgui.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"
#include <filesystem>
#include <fstream>
#include "Renderer/Renderer.h"
#include "Misc/ConsoleWindow.h"

namespace fs = std::filesystem;

ParticleEditor::ParticleEditor()
{
    open = false;
    gridColor = sf::Color::Magenta;
    grid = generateGrid(gridColor);
    repeating = false;
    color = sf::Color(variables.color[0], variables.color[1], variables.color[2], variables.color[3]);
    this->currentEmitter = -1;
    this->selectedKeyFrame = 0;
    this->settingStart = false;
    this->settingStop = false;
}

void ParticleEditor::update(sf::Vector2f mousePosWorld, float dt)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        emitto.setEmitterPos(mousePosWorld);
    
    //imgui block
    {
        ImGui::Begin("Pickle a Particle", &this->open);
        ImGui::BeginTabBar("Lekrutan");

        selectedKeyFrame = emitto.getPrev();

        ImGui::Text("Current frame: %d", selectedKeyFrame);

        if (ImGui::InputInt("Step to prev/next", &selectedKeyFrame, 1, 1))
        {
            selectedKeyFrame = std::max(0, selectedKeyFrame);
            selectedKeyFrame = std::min(emitto.getKeyFrameCount() - 1, selectedKeyFrame);

            emitto.setElapsedTime(emitto.getKeyFramePtr(selectedKeyFrame)->timeStamp + 1);
        }

        Emitter::KeyFrame* frame = emitto.getKeyFramePtr(selectedKeyFrame);

        float elapsedTime = emitto.getElapsedTime();
        float max = std::max(emitto.getEmitterLifeSpan(), emitto.getKeyFramePtr(emitto.getKeyFrameCount() - 1)->timeStamp) + 1;
        if (ImGui::SliderFloat("Timelapse", &elapsedTime, 0, max))
            emitto.setElapsedTime(elapsedTime);

        ImGui::SameLine();
        static bool freeze = false;
        if (ImGui::Checkbox("Freeze", &freeze))
            emitto.setFreeze(freeze);

        ImGui::Separator();

        if (ImGui::BeginTabItem("Particles"))
        {
            ImGui::DragFloat("Emitter lifespan", &variables.lifeSpan, 1, 0, INT_MAX);
            ImGui::Checkbox("Enable collision", &variables.collisionOn);
            ImGui::DragInt("Initial particles", &variables.initailParticles, 1, 0, INT_MAX);
            ImGui::Separator();

            variables.color[0] = frame->color.r / 255.f;
            variables.color[1] = frame->color.g / 255.f;
            variables.color[2] = frame->color.b / 255.f;
            variables.color[3] = frame->color.a / 255.f;

            variables.colorDev[0] = frame->colorDeviation.r / 255.f;
            variables.colorDev[1] = frame->colorDeviation.g / 255.f;
            variables.colorDev[2] = frame->colorDeviation.b / 255.f;
            variables.colorDev[3] = frame->colorDeviation.a / 255.f;

            ImGui::DragFloat("Particle lifespan", &frame->particleLifespan, 1, 0, INT_MAX);
            ImGui::DragFloat("Particle speed", &frame->speed, 0.01f, -INT_MAX, INT_MAX);
            ImGui::DragFloat("Spawn rate", &frame->spawnRate, 1, 0, INT_MAX);
            ImGui::DragInt("Particle per spawn", &frame->particlesPerSpawn, 1, 1, INT_MAX);
            ImGui::DragFloat("Size x", &frame->size.x, 0.05f, 0, INT_MAX);
            ImGui::DragFloat("Size y", &frame->size.y, 0.05f, 0, INT_MAX);
            ImGui::DragFloat("Offset", &frame->offset, 1, 0);
            ImGui::DragInt("Angle", &frame->emitterAngle, 1, 0, 360);
            ImGui::DragInt("Cone", &frame->emitterCone, 1, 1, 360);
            ImGui::ColorEdit4("Color", variables.color);
            ImGui::ColorEdit4("Color Deviation", variables.colorDev);
            ImGui::DragFloat("Friction", &frame->frictionValue, 0.001, 0, 2);
            ImGui::DragFloat("Jitter", &frame->jitterAmount, 0.001, 0, 1);
            ImGui::Checkbox("Enable gravity", &frame->affectedByGravity);
            ImGui::SameLine();
            ImGui::Checkbox("Follows center", &frame->followsCenter);
            ImGui::DragFloat("Gravity", &frame->gravity, 0.0001f, -1, 1, "%4f");

            color.r = variables.color[0] * 255;
            color.g = variables.color[1] * 255;
            color.b = variables.color[2] * 255;
            color.a = variables.color[3] * 255;
            frame->color = color;

            color.r = variables.colorDev[0] * 255;
            color.g = variables.colorDev[1] * 255;
            color.b = variables.colorDev[2] * 255;
            color.a = variables.colorDev[3] * 255;
            frame->colorDeviation = color;


            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Lighting"))
        {
            if (ImGui::Button("Add light"))
            {
                Emitter::EmitterLight light(emitto.getEmitterPos(), 100, sf::Vector3f(1, 1, 1));
                frame->lights.push_back(light);
            }

            ImGui::DragFloat("Particle light radius", &frame->particleLightRadius, 1, 0, 200);
            ImGui::Separator();
            ImGui::Separator();
            ImGui::BeginChild("LightList", sf::Vector2i(0, -200));
            
            for (size_t i = 0; i < frame->lights.size(); i++)
            {
                std::string collabel = "color " + std::to_string(i);
                std::string poslabel = "pos " + std::to_string(i);
                std::string radiuslabel = "radius " + std::to_string(i);

                Emitter::EmitterLight* light = &frame->lights[i];

                float col[3] = { light->initialColor.x, light->initialColor.y, light->initialColor.z };
                float pos[2] = { light->initialOffset.x, light->initialOffset.y };
                float radius = light->initialRadius;

                //Gotta make a new light if radius is altered. Thanks shadows
                if (ImGui::DragFloat(radiuslabel.c_str(), &radius, 1, 10, 3000))
                {
                    light->initialRadius = radius;
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


                light->initialOffset.x = pos[0];
                light->initialOffset.y = pos[1];
                light->light->pos = emitto.getEmitterPos() + light->initialOffset;

                if (ImGui::Button("Delete"))
                {
                    frame->lights.erase(frame->lights.begin() + i--);
                }
                ImGui::Separator();
            }


            ImGui::EndChild();
            ImGui::Separator();


            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Tendrils"))
        {
            if (ImGui::Button("Add tendie"))
            {
                frame->tendrils.push_back(Emitter::EmitterTendril());
            }



            ImGui::Separator();
            if (ImGui::BeginTabBar("tendrils"))
            {
                if (ImGui::BeginTabItem("Particle Tendrils"))
                {
                    ImGui::DragFloat("Interval", &frame->tendrilGenInterval);
                    if (frame->particleHasTendrils)
                        showTendril(&frame->particleTendril, frame->tendrilsGenerated, mousePosWorld);

                    ImGui::Checkbox("Particles has tendrils", &frame->particleHasTendrils);
                    ImGui::EndTabItem();
                }


                for (int i = 0; i < frame->tendrils.size(); i++)
                {
                    if (ImGui::BeginTabItem(("#" + std::to_string(i)).c_str()))
                    {
                        showTendril(&frame->tendrils[i], frame->tendrilsGenerated, mousePosWorld);

                        if (ImGui::Button("Erase tendie"))
                        {
                            frame->tendrils.erase(frame->tendrils.begin() + i--);
                        }
                        ImGui::EndTabItem();
                    }

                }

                ImGui::EndTabBar();
            }


            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Keyframes"))
        {
            float min = 0;
            ImGui::BeginChild("Achildname", sf::Vector2f(0, -200));
            for (int i = 0; i < emitto.getKeyFrameCount(); i++)
            {
                if (i > 0)
                    min = emitto.getKeyFramePtr(i - 1)->timeStamp + 1;

                    emitto.getKeyFramePtr(i)->timeStamp = std::max(min, emitto.getKeyFramePtr(i)->timeStamp);

                std::string label = "Timestamp #" + std::to_string(i);
                ImGui::DragFloat(label.c_str(), &emitto.getKeyFramePtr(i)->timeStamp, 1, min, 1000000);
            }
            ImGui::EndChild();
            ImGui::Separator();
            if (ImGui::Button("Add keyframe"))
                emitto.addKeyFrame(++selectedKeyFrame);


            if (emitto.getKeyFrameCount() > 1)
            {
                ImGui::SameLine();
                if (ImGui::Button("Remove keyframe"))
                {
                    emitto.removeKeyFrame(selectedKeyFrame--);
                    selectedKeyFrame = std::max(selectedKeyFrame, 0);
                }
            }

            ImGui::EndTabItem();
        }

       /* if (ImGui::BeginTabItem("Editor"))
        {
            float col[3] = { gridColor.r, gridColor.g, gridColor.b };

            if (ImGui::DragFloat3("grid color", col, 1, 0, 255))
            {
                gridColor = sf::Color(col[0], col[1], col[2]);
                grid = generateGrid(gridColor);
            }

            //ImGui::DragFloat("Zooooom", &zoomLevel, 1, 1, 8);

            ImGui::EndTabItem();
        }*/

        if (ImGui::BeginTabItem("Experimental"))
        {
            if (ImGui::Button("save all!!!"))
                saveAll();

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();

        ImGui::Separator();
        ImGui::DragInt3("Background color", variables.clearColor, 1, 0, 255);
        ImGui::Checkbox("Repeating", &repeating);
        ImGui::SameLine();
        if (ImGui::Checkbox("Enable particle lights!?", &variables.hasLight))
            emitto.setParticleHasLight(variables.hasLight);

        ImGui::Text("Emitter: %s,", fileNames[currentEmitter].c_str());
        ImGui::SameLine();
        ImGui::Text("Particles: %d,", emitto.getParticleCount());
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
            restart(&emitto);

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

    emitto.setEmitterLifeSpan(variables.lifeSpan);
    emitto.setInitialParticles(variables.initailParticles);
    emitto.setColliding(variables.collisionOn);



    if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
        emitto.kill();

    updateTimer.restart();
    emitto.update(dt);
    particleUpdateTime = updateTimer.getElapsedTime();



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

void ParticleEditor::showTendril(Emitter::EmitterTendril* tendril, bool& generated, sf::Vector2f mousePosWorld)
{
    Tendril::InitGenData* data = &tendril->tendrilData;
    static const sf::Color ACTIVE_COLOR = sf::Color(100, 200, 200);
    static bool popStyle = false;

    float col[4] = { data->color.r / 255.f, data->color.g / 255.f, data->color.b / 255.f, data->color.a / 255.f };

    if (settingStart)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ACTIVE_COLOR);
        popStyle = true;
    }

    if (ImGui::Button("Set start pos"))
    {
        this->settingStart = !settingStart;
        this->settingStop = false;
    }

    if (popStyle)
    {
        ImGui::PopStyleColor();
        popStyle = false;
    }

    if (settingStop)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ACTIVE_COLOR);
        popStyle = true;
    }

    if (ImGui::Button("Set end pos"))
    {
        this->settingStop = !settingStop;
        this->settingStart = false;
    }

    if (popStyle)
    {
        ImGui::PopStyleColor();
        popStyle = false;
    }


    if (ImGui::ColorEdit4("Color", col))
    {
        generated = false;
        data->color = sf::Color(col[0] * 255.f, col[1] * 255.f, col[2] * 255.f, col[3] * 255.f);
    }

    if (ImGui::DragInt("Splits", &data->splits))
        generated = false;
    if (ImGui::DragInt("sway", &data->sway))
        generated = false;
    if (ImGui::DragInt("min", &data->min))
        generated = false;
    if (ImGui::DragInt("max", &data->max))
        generated = false;
    if (ImGui::DragInt("posMin", &data->forkMin))
        generated = false;
    if (ImGui::DragInt("posMax", &data->forkMax))
        generated = false;
    if (ImGui::DragFloat("Height", &data->peakHeight, 0.001f))
        generated = false;
    if (ImGui::DragInt("fork degrees", &data->angle, 1.f))
        generated = false;
    if (ImGui::DragInt("Visible time", &data->visibleTime, 1.f))
        generated = false;
    if (ImGui::DragFloat("Fadespeed", &data->fadeSpeed, 0.01f))
        generated = false;
    if (ImGui::DragFloat("thickness", &data->thickness, 1.f))
        generated = false;
    if (ImGui::InputInt("Algorithm", &data->timeAlgorithm))
        generated = false;
    if (ImGui::Checkbox("repeating", &data->repeating))
        generated = false;


    if (data->splits < 0)
        data->splits = 0;

    if (data->min < 0)
        data->min = 0;

    if (data->max < data->min)
        data->max = data->min;

    if (data->forkMax > data->splits)
        data->forkMax = data->splits;

    if (data->forkMin < 0)
        data->forkMin = 0;

    if (data->forkMax < data->forkMin)
        data->forkMax = data->forkMin;

    if (settingStart || settingStop)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (settingStart)
                tendril->start = mousePosWorld - emitto.getEmitterPos();

            else if (settingStop)
                tendril->stop = mousePosWorld - emitto.getEmitterPos();

            settingStart = false;
            settingStop = false;
            generated = false;
        }
    }
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

void ParticleEditor::saveAll()
{
    for (int i = 0; i < fileNames.size(); i++)
    {
        load(i);

        std::ofstream file(emitterFolder + fileNames[i]);

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
        this->selectedKeyFrame = 0;
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
    variables = { 30000, 30,
        0, 255, 0, 255,
        0, 0, 0, 0,
        0, 0, 0, 
        false,
        false};

    *emitter = Emitter();

}