#pragma once
#include <string>
#include <istream>
#include <ostream>
#include "Game/Entities/Entity.h"
#include "Game/Components/SpriteComp.h"


class Item : public Entity, public sf::Drawable
{
public:
    Item(sf::Vector2f pos, const sf::Texture* texture);
    virtual ~Item() {};

    friend std::istream& operator>>(std::istream& in, Item& item);
    friend std::ostream& operator<<(std::ostream& out, const Item& item);


    int getID() const { return id; };
    void setID(int id) { this->id = id; };

    std::string getName() const { return name; };
    void setName(std::string name) { this->name = name; };

    int getStackLimit() const { return stackLimit; };
    void setStackLimit(int size) { this->stackLimit = size; };

    int getEmitterID() const { return emitterID; };
    void setEmitterID(int emitterID) { this->emitterID = emitterID; };

    bool isUseable()const { return useable; };
    void setUseable(bool useable) { this->useable = useable; };

    void pluck() { this->obtained = true; }; //I just wanted to name it like that it could have been called obtainItem or something like that but nonono
    bool isObtained() const { return obtained; };

    const sf::Texture* getTexture() const { return this->sprite.getTexture(); };
    void setTexture(const sf::Texture* texture) { this->sprite.setTexture(texture); };

    sf::Vector2f getTextureCenterPos() const { return getPosition() + (sf::Vector2f(sprite.getTexture()->getSize()) / 2.f); };
    sf::Vector2f getTextureSize() const { return sf::Vector2f(sprite.getTexture()->getSize()); };

    void setPosition(sf::Vector2f pos);
protected:
    SpriteComp sprite;

private:
    int id;
    std::string name;
    int stackLimit;
    int emitterID;
    bool useable;
    bool obtained;


    virtual std::istream& readSpecific(std::istream& in) { return in; };
    virtual std::ostream& writeSpecific(std::ostream& out)const { return out; };

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

