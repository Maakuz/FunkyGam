#pragma once
#include "SFML/Graphics.hpp"
#include "Renderer/Lighting/ShadowHandler.h"
#include "Renderer/ShaderHandler.h"
#include "Renderer/DebugDrawable.h"
#include <vector>

#define NR_OF_RENDER_TARGETS 3

class Renderer
{
public:
    Renderer(sf::Window* window);
    Renderer(Renderer&) = delete;
    ~Renderer() {};

    void render(sf::RenderTarget& target);
    void renderDebug(sf::RenderTarget& target);

    static void queueDrawable(const sf::Drawable* drawable) { queue.push_back(drawable); };
    static void queueDebug(const DebugDrawable* drawable) { debugQueue.push_back(drawable); };
    static void queueUI(const sf::Drawable* drawable) { uiQueue.push_back(drawable); };
    static void setDrawLights(bool drawLights) { Renderer::drawLights = drawLights; };
private:
    static std::vector<const sf::Drawable*> uiQueue; 
    static std::vector<const sf::Drawable*> queue;
    static std::vector<const DebugDrawable*> debugQueue;
    sf::RectangleShape fullscreenboi;
    sf::View uiView;

    ShaderHandler shaderHandler;
    ShadowHandler shadowHandler;

    static bool drawLights;
    bool drawLightMap;

    sf::RenderTexture renderTargets[NR_OF_RENDER_TARGETS];
};
