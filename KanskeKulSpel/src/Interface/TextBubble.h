#pragma once
#include "SFML/Graphics.hpp"

class TextBubble : public sf::Drawable
{
public:
    TextBubble(sf::Texture* texture);
    TextBubble();
    ~TextBubble() {};

    void create(sf::Texture* texture);
    void setTexture(sf::Texture* texture) { this->texture = texture; };//this only works if the textures has the same dimensions. TAKE HEED
    void setWidth(unsigned int width);
    void setHeight(unsigned int height);
    sf::Vector2u resizeToFit();
    void setPos(sf::Vector2f pos);
    void setFont(sf::Font* font);
    void setCharacterSize(unsigned int size);
    void setText(std::string text);
    void appendText(std::string text);
    sf::FloatRect getBounds();

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