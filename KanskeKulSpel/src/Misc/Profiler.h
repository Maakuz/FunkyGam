#pragma once
#include <vector>
#include "SFML/System/Clock.hpp"
#include <string>
#include "Imgui/imgui.h"

class Profiler
{
public:
    static Profiler& get()
    {
        static Profiler profiler;
        return profiler;
    }
    ~Profiler() {};

    void drawGUI()
    {
        std::string text = "test" + std::to_string(4.f);
        ImGui::Begin("Pröööf");
        ImGui::Button(text.c_str());
        ImGui::End();
    }

    void start(std::string title)
    {
        timer.restart();
        names.push_back(title);
    }

    void stop()
    {
        times.push_back(timer.getElapsedTime().asMicroseconds());
    }

private:
    sf::Clock timer;
    std::vector<float> times;
    std::vector<std::string> names;
};

