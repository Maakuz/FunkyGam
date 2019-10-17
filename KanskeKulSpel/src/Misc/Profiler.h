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

        if (show == nullptr || *show)
        {
            std::string text = "Profolio, MicSec!";
            ImGui::Begin(text.c_str(), NULL, sf::Vector2f(500, 500));
            text = "Framratio: " + std::to_string(1000 / deltaTime);
            ImGui::Text(text.c_str());

            sf::Vector2f pos = ImGui::GetCursorPos();
            for (auto& it : head->entries)
            {
                recursivelyBuildList(it.second, pos);
                pos.x += it.second->getTime() / 50 + (spacingX * it.second->entries.size() + spacingX);
                ImGui::SetCursorPos(pos);
            }

            ImGui::End();
        }

    }

    void start(std::string title)
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

    void stop()
    {
        assert(currentEntry != head); //Missing start

        currentEntry->times[currentEntry->i++] = (float)currentEntry->timer.getElapsedTime().asMicroseconds();
        
        if (currentEntry->i >= SAVED_TIMES)
            currentEntry->i = 0;

        currentEntry = currentEntry->prev;
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

    Profiler()
    {
        this->currentEntry = nullptr;
        spacingX = 2;
        this->head = new Entry();
        this->head->name = "Head";
        this->head->prev = nullptr;

        this->currentEntry = head;
    }

    void recursivelyBuildList(Entry* entry, sf::Vector2f pos)
    {
        std::string text;
        float time = entry->getTime();
        sf::Vector2f size(time / 50 + (spacingX * entry->entries.size()), 20);
        text = entry->name + ", " + std::to_string(time);
        ImGui::SetCursorPos(pos);
        ImGui::Button(text.c_str(), size);

        pos.y += size.y;
        for (auto& next : entry->entries)
        {
            recursivelyBuildList(next.second, pos);
            pos.x += next.second->getTime() / 50 + spacingX;
        }
    }


};

