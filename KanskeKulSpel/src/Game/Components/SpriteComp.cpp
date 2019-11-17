#include "SpriteComp.h"

SpriteComp::SpriteComp(const sf::Texture* texture, sf::Vector2f pos)
{
    flipped = false;

    sprite.setPosition(pos);

    if (texture)
        sprite.setTexture(*texture, true);
}

void SpriteComp::flipHorizontally()
{
    sf::IntRect rect = sprite.getTextureRect();
    rect.width = -rect.width;
    sprite.setTextureRect(rect);
    flipped = !flipped;
}

void SpriteComp::update(sf::Vector2f pos)
{
    this->sprite.setPosition(pos + spriteOffset);
}

void SpriteComp::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite, states);
}
