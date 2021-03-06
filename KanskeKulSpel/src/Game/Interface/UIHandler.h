#pragma once
#include "InventoryHandler.h"
#include "Game/Misc/Counter.h"

class UIHandler : public sf::Drawable
{
public:
    UIHandler();
    virtual ~UIHandler();

    void initialize();

    void update(float dt, sf::Vector2f mousePos);

    void displayNewItem(int item);
    void displayEnemyDamage(float percentage);
    void hideEnemyDamage();

    const sf::View& getView() const { return this->view; };

    InventoryHandler* getInventory() { return &this->inventory; };
    void setHealthPercentage(float percentage) { this->healthPercentage = percentage; };
private:

    std::vector<Item*> stash;
    std::vector<sf::RectangleShape> stashSlots;
    InventoryHandler inventory;

    TextBubble newItem;
    Counter newItemDisplayDuration;
    Counter showEnemyHealth;

    sf::Vector2u slotSize;
    sf::Sprite healthBarOverlay;
    sf::Vertex healthBar[4];
    sf::Sprite enemyHealthBarOverlay;
    sf::Vertex enemyHealthBar[4];
    sf::Texture* healthTexture;
    float healthPercentage;

    const sf::Texture* slotTexture;

    sf::View view;



    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};