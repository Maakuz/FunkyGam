#pragma once
#include "SFML/System/Vector2.hpp"
#include "Game/Components/Comp.h"

class TransformComp : public Comp
{
public:
    TransformComp(sf::Vector2f pos = sf::Vector2f()) { this->pos = pos; };
    virtual ~TransformComp() {};

    static ComponentKey getStaticKey() { return ComponentKey::transform; };
    virtual Comp* clone() const { return new TransformComp(*this); };

    sf::Vector2f pos;
};