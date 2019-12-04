#pragma once
#include <string>
#include <istream>
#include <ostream>
#include "Game/Components/SpriteComp.h"
#include "Game/Components/TransformComp.h"
#include "Game/Entities/Entity.h"
#include "Game/Components/LogisticsComp.h"


class Item: public Entity, public sf::Drawable
{
public:
    Item(sf::Vector2f pos, const sf::Texture* texture);
    virtual ~Item() {};

    friend std::istream& operator>>(std::istream& in, Item& item);
    friend std::ostream& operator<<(std::ostream& out, const Item& item);

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

