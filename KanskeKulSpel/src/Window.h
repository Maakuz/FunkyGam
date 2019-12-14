#pragma once
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Game/Game.h"
#include "Imgui/imgui.h"
#include "Imgui/SFML-imgui/imgui-SFML.h"
#include "Misc/ConsoleWindow.h"
#include "Renderer/Renderer.h"
#include "LevelEditor/Editor.h"
#include "Misc/Profiler.h"

class Window
{
public:
    Window();
    virtual ~Window();

    int run();

private:
    enum class State
    {
        game,
        editor
    };
    State m_state;

    sf::RenderWindow m_wandow;
    sf::Color m_clearColor;
    Game* m_game;
    Editor* m_editor;
    Renderer* m_renderer;
    sf::Clock m_deltaTimer;
    sf::Time m_deltaTime;


    bool m_profilerActive;

    bool m_debugActive;
    bool m_debugActivePrev;

    bool m_focused;
    bool m_exitPromptActive;
    bool m_printScreen;

    void handlePollEvents();
    void takePrintScreen();
    void pause();
    void resume();

    bool exitPrompt();
};