#pragma once
#include "SFML/Graphics.hpp"
#include "Handlers/ShadowHandler.h"
#include "Handlers/ShaderHandler.h"
#include <vector>

#define NR_OF_RENDER_TARGETS 3

class Renderer
{
public:
    Renderer(sf::Window* window);
    ~Renderer() {};

    void render(sf::RenderTarget& target);
    void renderDebug(sf::RenderTarget& target);

    static void queueDrawable(sf::Drawable* drawable) { queue.push_back(drawable); };
    static void queueUI(sf::Drawable* drawable) { uiQueue.push_back(drawable); };
private:
    static std::vector<sf::Drawable*> uiQueue; 
    static std::vector<sf::Drawable*> queue;
    sf::RectangleShape fullscreenboi;
    sf::View uiView;

    ShaderHandler shaderHandler;
    ShadowHandler shadowHandler;

    sf::RenderTexture renderTargets[NR_OF_RENDER_TARGETS];
};
