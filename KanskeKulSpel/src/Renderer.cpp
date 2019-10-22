#include "Renderer.h"
#include "Misc/Profiler.h"

std::vector<sf::Drawable*> Renderer::queue;
std::vector<sf::Drawable*> Renderer::uiQueue;

Renderer::Renderer(sf::Window* window)
{
    this->fullscreenboi = sf::RectangleShape(sf::Vector2f(window->getSize()));
    this->fullscreenboi.setPosition(0, 0);

    for (int i = 0; i < NR_OF_RENDER_TARGETS; i++)
    {
        this->renderTargets[i].create(window->getSize().x, window->getSize().y);
    }

    this->uiView.setSize(sf::Vector2f(window->getSize()));
    this->uiView.setCenter(sf::Vector2f(window->getSize()) / 2.f);

}

void Renderer::render(sf::RenderTarget& target)
{
    PROFILER_START("Drawqueue");

    for (sf::Drawable* drawable : queue)
        target.draw(*drawable);

    queue.clear();
    PROFILER_STOP;


    //Shadow map
    PROFILER_START("Shoaduv");
    sf::View view = target.getView();
    this->fullscreenboi.setPosition(view.getCenter() - (view.getSize() / 2.f));
    sf::Vector2f offset = view.getCenter() - (view.getSize() / 2.f);
    
    this->renderTargets[0].clear(sf::Color::Black);
    this->renderTargets[0].setView(view);
    this->shadowHandler.generateShadowMap(this->renderTargets[0], offset);
    this->renderTargets[0].display();

    this->fullscreenboi.setTexture(&this->renderTargets[0].getTexture());

    for (int i = 0; i < 2; i++)
    {
        this->renderTargets[i + 1].setView(view);
        this->renderTargets[i + 1].clear(sf::Color::Transparent);
        this->renderTargets[i + 1].draw(this->fullscreenboi, &shaderHandler[shaderHandler.BLUR_PASS[i]]);
        this->renderTargets[i + 1].display();

        this->fullscreenboi.setTexture(&this->renderTargets[i + 1].getTexture());
    }

    target.draw(fullscreenboi, sf::BlendMultiply);
    PROFILER_STOP;

    PROFILER_START("No shadow lights render");
    renderTargets[0].clear(sf::Color::Black);
    for (size_t i = 0; i < LightQueueNoShadow::get().getQueue().size(); i++)
    {
        Light_NoShadow* light = LightQueueNoShadow::get().getQueue()[i];
        ShaderHandler::getShader(SHADER::lightingNoShadow).setUniform("pos", light->pos - offset);
        ShaderHandler::getShader(SHADER::lightingNoShadow).setUniform("radius", light->radius);
        ShaderHandler::getShader(SHADER::lightingNoShadow).setUniform("color", light->color);

        sf::RenderStates state;
        state.shader = &ShaderHandler::getShader(SHADER::lightingNoShadow);
        state.blendMode = sf::BlendAdd;


        sf::RectangleShape sprite(sf::Vector2f(light->radius * 2, light->radius * 2));
        sprite.setPosition(light->pos - (sf::Vector2f(sprite.getSize() / 2.f)));
        renderTargets[0].draw(sprite, state);
    }
    renderTargets[0].display();

    fullscreenboi.setTexture(&renderTargets[0].getTexture());

    for (int i = 0; i < 2; i++)
    {
        renderTargets[i + 1].clear(sf::Color::Transparent);
        renderTargets[i + 1].draw(fullscreenboi, &shaderHandler[shaderHandler.BLUR_PASS[i]]);
        renderTargets[i + 1].display();

        fullscreenboi.setTexture(&renderTargets[i + 1].getTexture());
    }

    target.draw(fullscreenboi, sf::BlendAdd);
    PROFILER_STOP;

    LightQueue::get().clear();
    LightQueueNoShadow::get().clear();

    //Render UI after all light nonsens
    sf::View temp = target.getView();
    target.setView(uiView);

    for (sf::Drawable* drawable : uiQueue)
        target.draw(*drawable);

    uiQueue.clear();

    target.setView(temp);
}