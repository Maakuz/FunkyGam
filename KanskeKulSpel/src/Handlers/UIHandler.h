#pragma once
#include "SFML/Graphics.hpp"


class UIHandler : public sf::Drawable
{
public:
    UIHandler();
    ~UIHandler() {};

    void initialize();

    const sf::View& getView() const { return this->view; };
    void setSelectedItem(int item);

private:
    sf::VertexArray quickslotVertexArray;
    sf::Vector2u quickslotSize;
    sf::Vector2u quickslotPos;
    int quickslotCount;
    int selectedItem;

    sf::View view;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};