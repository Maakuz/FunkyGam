#pragma once
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Texture.hpp"

class SpriteComp : public sf::Drawable
{
public:
    SpriteComp(const sf::Texture* texture, sf::Vector2f pos = sf::Vector2f(0, 0));
    ~SpriteComp() {};

    virtual void flipHorizontally();
    bool isFlippedHorizontally() const { return flipped; };

    void setTextureRect(sf::IntRect rect) { sprite.setTextureRect(rect); };
    const sf::IntRect& getTextureRect() { return this->sprite.getTextureRect(); };

    void setOrigin(sf::Vector2f origin) { sprite.setOrigin(origin); };

    void setRotation(float angle) { this->sprite.setRotation(angle); };

    void update(sf::Vector2f pos);

    void setTexture(const sf::Texture* texture) { this->sprite.setTexture(*texture); };
    const sf::Texture* getTexture() const { return this->sprite.getTexture(); };

    sf::Vector2f spriteOffset;

private:
    sf::Sprite sprite;
    bool flipped;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};