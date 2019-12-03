#pragma once
#include <string>
#include <istream>
#include <ostream>
#include "Game/Components/SpriteComp.h"
#include "Game/Components/TransformComp.h"
#include "Game/Entities/Entity.h"


class Item: public Entity, public sf::Drawable
{
public:
    Item(sf::Vector2f pos, const sf::Texture* texture);
    virtual ~Item() {};

    const sf::Texture* getTexture() const { return this->sprite.getTexture(); };
    void setTexture(const sf::Texture* texture) { this->sprite.setTexture(texture); };

    //this only works if origin is unchanged
    sf::Vector2f getTextureCenterPos() const { return sprite.getPosition() + (sf::Vector2f(sprite.getTexture()->getSize()) / 2.f); };
    sf::Vector2f getTextureSize() const { return sf::Vector2f(sprite.getTexture()->getSize()); };

    void setPosition(sf::Vector2f pos);
private:



    virtual std::istream& readSpecific(std::istream& in) { return in; };
    virtual std::ostream& writeSpecific(std::ostream& out)const { return out; };

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

