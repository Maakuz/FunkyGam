#pragma once
#include "SFML/Graphics.hpp"

class TextBubble : public sf::Drawable
{
public:
    TextBubble(sf::Texture* texture);
    TextBubble();
    ~TextBubble() {};

    void setTexture(sf::Texture* texture);
    void setWidth(unsigned int width);
    void setHeight(unsigned int height);
    void setPos(sf::Vector2f pos);
    void setFont(sf::Font* font);
    void setCharacterSize(unsigned int size);
    void setText(std::string text);
    void appendText(std::string text);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    
    float width;
    float height;
    sf::Vector2u size;
    sf::Texture* texture;
    sf::Vector2f pos;
    sf::Transform transform;

    sf::VertexArray bubble;
    sf::Text text;
};