#pragma once
#include <map>
#include "SFML/System/Clock.hpp"
#include <string>
#include "Imgui/imgui.h"

#define PROFILER_START(name) Profiler::get().start(name)
#define PROFILER_STOP Profiler::get().stop()

class Profiler
{
public:
    static Profiler& get()
    {
        static Profiler profiler;
        return profiler;
    }
    ~Profiler() 
    {
        delete head;
    };

    void updateProfiler(float deltaTime = 0, bool* show = nullptr)
    {
        assert(currentEntry == head); //missing stop

        if (counter > updateFrequency)
            counter = 0;

        counter += deltaTime;
        iterations++;

        if (show == nullptr || *show)
        {
            std::string text = "Profolio, MicSec!";
            ImGui::Begin(text.c_str(), show, sf::Vector2f(500, 500));
            text = "Framratio: " + std::to_string(1000 / deltaTime);
            ImGui::Text(text.c_str());

            const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
            if (ImGui::BeginChild(ImGui::GetID("Child"), sf::Vector2f(0, -footer_height_to_reserve * 3)))
            {
                sf::Vector2f pos = ImGui::GetCursorPos();
                pos.x += scroll;
                for (auto& it : head->entries)
                {
                    recursivelyBuildList(it.second, pos);
                    pos.x += it.second->getTime() / zoom + (spacingX * it.second->entries.size() + spacingX);
                    ImGui::SetCursorPos(pos);
                }
                ImGui::EndChild();

                ImGui::DragFloat("Scroll", &scroll, 1, -100000, 100000);
                ImGui::DragFloat("Zoom", &zoom, 0.1, 0.01, 100);
                ImGui::DragFloat("Update freq", &updateFrequency, 10, 0, 10000);
            }
            ImGui::End();
        }

    }

    void start(std::string title)
    {
        if (counter > updateFrequency || iterations < SAVED_TIMES)
        {
            if (currentEntry->entries.count(title) > 0)
            {
                currentEntry = currentEntry->entries[title];
                currentEntry->timer.restart();
            }

            else
            {
                Entry* entry = new Entry();
                entry->name = title;
                entry->prev = currentEntry;
                entry->timer.restart();
                currentEntry->entries.insert({ title, entry });
                currentEntry = entry;
            }
        }
            
    }

    void stop()
    {
        if (counter > updateFrequency || iterations < SAVED_TIMES)
        {
            assert(currentEntry != head); //Missing start

            currentEntry->times[currentEntry->i++] = (float)currentEntry->timer.getElapsedTime().asMicroseconds();

            if (currentEntry->i >= SAVED_TIMES)
                currentEntry->i = 0;

            currentEntry = currentEntry->prev;
        }
    }

private:
    static const int SAVED_TIMES = 10;
    struct Entry
    {
        std::string name;
        sf::Clock timer;
        float times[SAVED_TIMES];
        int i;
        std::map<std::string, Entry*> entries;
        Entry* prev;
        Entry()
        {
            prev = nullptr;
            for (int j = 0; j < SAVED_TIMES; j++)
            {
                times[j] = 0;
            }
            i = 0;
        }

        ~Entry()
        {
            for (auto& it: entries)
                delete it.second;
        }

        float getTime()
        {
            float time = 0;
            for (int j = 0; j < SAVED_TIMES; j++)
            {
                time += times[j];
            }
            time /= SAVED_TIMES;
            return time;
        }
    };

    Entry* head;
    Entry* currentEntry;
    float spacingX;
    float minSizeX;
    float zoom;
    float updateFrequency;
    float counter;
    int iterations;
    float scroll;

    Profiler()
    {
        this->currentEntry = nullptr;
        spacingX = 2;
        minSizeX = 30;
        this->head = new Entry();
        this->head->name = "Head";
        this->head->prev = nullptr;

        this->currentEntry = head;
        zoom = 10;
        updateFrequency = 1000;
        counter = 0;
        iterations = 0;
        scroll = 0;
    }

    void recursivelyBuildList(Entry* entry, sf::Vector2f pos)
    {
        std::string text;
        float time = entry->getTime();
        if (time > 0.0001)
        {
            sf::Vector2f size(time / zoom + (spacingX * entry->entries.size()), 20);
            text = entry->name + ", " + std::to_string(time);
            ImGui::SetCursorPos(pos);
            ImGui::Button(text.c_str(), size);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", text.c_str());
            pos.y += size.y;
        }

        for (auto& next : entry->entries)
        {
            recursivelyBuildList(next.second, pos);
            pos.x += next.second->getTime() / zoom + spacingX;
        }
    }


};

