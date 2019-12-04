#pragma once
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "Game/Components/Comp.h"

class SpriteComp : public Comp, public sf::Drawable
{
public:
    SpriteComp(const sf::Texture* texture, sf::Vector2f pos = sf::Vector2f(0, 0));
    virtual ~SpriteComp() {};

    static ComponentKey getStaticKey() { return ComponentKey::sprite; };
    virtual Comp* clone() const { return new SpriteComp(*this); };

    virtual void flipHorizontally();
    bool isFlippedHorizontally() const { return flipped; };

    void setTextureRect(sf::IntRect rect) { sprite.setTextureRect(rect); };
    const sf::IntRect& getTextureRect() { return this->sprite.getTextureRect(); };

    void setOrigin(sf::Vector2f origin) { sprite.setOrigin(origin); };

    void setRotation(float angle) { this->sprite.setRotation(angle); };

    void setPosition(sf::Vector2f pos);

    sf::Vector2f getPosition()const { return sprite.getPosition(); };

    void setTexture(const sf::Texture* texture) { this->sprite.setTexture(*texture); };
    const sf::Texture* getTexture() const { return this->sprite.getTexture(); };

    //this only works if origin and texturerect is unchanged
    virtual sf::Vector2f getTextureCenterPos() const { return this->sprite.getPosition() + (sf::Vector2f(this->sprite.getTexture()->getSize()) / 2.f); };
    virtual sf::Vector2f getTextureSize() const { return (sf::Vector2f)sprite.getTexture()->getSize(); };

    sf::Vector2f spriteOffset;

private:
    sf::Sprite sprite;
    bool flipped;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};