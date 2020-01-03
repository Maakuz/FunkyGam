#pragma once
#include "Game/Entities/Entity.h"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"
#include "Game/Components/SpriteComp.h"
#include "Game/Components/LogisticsComp.h"
#include "Game/Components/StatusComp.h"

class Consumable : public Entity, public sf::Drawable
{
public:
	Consumable(const sf::Texture* texture, sf::Vector2f pos = sf::Vector2f());
	virtual ~Consumable() {};

	friend std::istream& operator>>(std::istream& in, Consumable& item);
	friend std::ostream& operator<<(std::ostream& out, const Consumable& item);

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};