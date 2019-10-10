#pragma once
#include <vector>
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
    ~Profiler() {};

    void updateProfiler(float deltaTime = 0)
    {
        std::string text ="Profolio, MicSec!";
        ImGui::Begin(text.c_str(), NULL, sf::Vector2f(500, 500));
        text = "Framratio: " + std::to_string(1000 / deltaTime);
        ImGui::Text(text.c_str());

        for (size_t i = 0; i < times.size(); i++)
        {
            text = names[i] + ", " + std::to_string(times[i]);
            ImGui::Button(text.c_str());
        }
        ImGui::End();

        times.clear();
        names.clear();
    }

    void start(std::string title)
    {
        assert(!started); //Call stop after every call
        timer.restart();
        names.push_back(title);
        started = true;
    }

    void stop()
    {
        assert(started); //Start it first
        times.push_back((float)timer.getElapsedTime().asMicroseconds());
        started = false;
    }

private:
    sf::Clock timer;
    std::vector<float> times;
    std::vector<std::string> names;
    bool started;
};

