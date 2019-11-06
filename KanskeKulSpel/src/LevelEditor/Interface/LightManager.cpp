#include "LightManager.h"
#include "Imgui/imgui.h"
#include "Renderer/Lighting/LightQueue.h"
#include "SFML/Window/Mouse.hpp"
#include "LevelEditor/Constants.h"

const sf::Vector2f offset(TILEMENU_WIDTH, MENU_BAR_HEIGHT);

LightManager::LightManager()
{
    this->defaultRadius = 500;
    currentLight = 0;
    this->lightToBePlaced = nullptr;
    this->placingLight = false;
}

LightManager::~LightManager()
{
    for (Light* light : lights)
        delete light;
}

void LightManager::update(sf::Vector2i workMousePos, bool guiActive)
{
    if (placingLight)
    {
        lightToBePlaced->pos = sf::Vector2f(workMousePos);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !guiActive)
        {
            lights.push_back(lightToBePlaced);
            lightToBePlaced = nullptr;
            placingLight = false;
            currentLight = lights.size() - 1;
        }
    }
}

void LightManager::updateMenu()
{
    if (ImGui::BeginChild("id"))
    {
        if (ImGui::Button("Add light") && !placingLight)
        {
            lightToBePlaced = new Light(defaultLightPos, defaultRadius);
            placingLight = true;
        }

        ImGui::Separator();

        if (lights.size() > 0)
        {
            std::string current = std::to_string(lights[currentLight]->pos.x) + ", " + std::to_string(lights[currentLight]->pos.y);
            if (ImGui::BeginCombo("Current light", current.c_str()))
            {
                for (int i = 0; i < lights.size(); i++)
                {
                    std::string label = std::to_string(lights[i]->pos.x) + ", " + std::to_string(lights[i]->pos.y);
                    if (ImGui::Selectable(label.c_str()))
                    {
                        currentLight = i;
                    }

                    if (ImGui::IsItemHovered())
                    {
                        LightQueue::get().queue(lights[i]);
                        LightQueue::get().queue(lights[i]);
                    }
                }

                ImGui::EndCombo();
            }

            if (ImGui::Button("Move") && !placingLight)
            {
                lightToBePlaced = lights[currentLight];
                lights.erase(lights.begin() + currentLight);
                placingLight = true;
                currentLight = 0;
            }

            ImGui::SameLine();

            if (ImGui::Button("Copy") && !placingLight)
            {
                lightToBePlaced = new Light(lights[currentLight]->pos, lights[currentLight]->radius, lights[currentLight]->color);
                placingLight = true;
            }

            ImGui::SameLine();

            if (ImGui::Button("Delete") && !placingLight)
            {
                delete lights[currentLight];
                lights.erase(lights.begin() + currentLight);
                currentLight = lights.size() - 1;
            }

            if (lights.size() > 0)
            {
                float rad = lights[currentLight]->radius;
                if (ImGui::DragFloat("Radius", &rad, 1, 5, 10000) && !placingLight)
                    lights[currentLight]->radius = rad;


                sf::Vector3f col = lights[currentLight]->color;
                if (ImGui::ColorPicker3("Color", &col.x) && !placingLight)
                    lights[currentLight]->color = col;
            }
        }

        ImGui::EndChild();
    }
}

void LightManager::queueLights()
{
    for (Light* light : lights)
        LightQueue::get().queue(light);

    if (lightToBePlaced)
        LightQueue::get().queue(lightToBePlaced);
}

std::ostream& operator<<(std::ostream& out, const LightManager& obj)
{
    out << obj.lights.size() << " ";

    for (Light* light : obj.lights)
    {
        out << light->pos.x - offset.x << " " << light->pos.y - offset.y << " ";
        out << light->radius << " ";
        out << light->color.x << " " << light->color.y << " " << light->color.z << "\n";
    }


    return out;
}

std::istream& operator>>(std::istream& in, LightManager& obj)
{
    for (Light* light : obj.lights)
        delete light;

    obj.lights.clear();

    int size = 0;
    in >> size;

    for (int i = 0; i < size; i++)
    {
        sf::Vector3f col;
        float rad;
        sf::Vector2f pos;

        in >> pos.x >> pos.y;
        in >> rad;
        in >> col.x >> col.y >> col.z;


        Light* light = new Light(pos + offset, rad, col);
        obj.lights.push_back(light);
    }
    obj.currentLight = size - 1;

    return in;
}
