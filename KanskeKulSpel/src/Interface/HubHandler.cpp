#include "HubHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/MouseState.h"

#define OFF_TEX 8
#define ON_TEX 9

HubHandler::HubHandler(UIHandler* uiHandler)
{
    this->ui = uiHandler;
    this->background.setTexture(*TextureHandler::get().getTexture(6), true);
    this->background.setScale(2, 2);
    std::string mainButtonText[MAIN_BUTTON_COUNT] = {"Play level 1", "Manage inventory"};

    for (int i = 0; i < MAIN_BUTTON_COUNT; i++)
    {
        this->mainButtons[i].create(TextureHandler::get().getTexture(8), TextureHandler::get().getFont());
        this->mainButtons[i].setCharacterSize(16);
        this->mainButtons[i].setText(mainButtonText[i]);
        this->mainButtons[i].resizeToFit();
        this->mainButtons[i].setPos(sf::Vector2f(600, 100 + i * 100));
    }

    this->backButton.create(TextureHandler::get().getTexture(8), TextureHandler::get().getFont());
    this->backButton.setCharacterSize(16);
    this->backButton.setText("Back");
    this->backButton.resizeToFit();
    this->backButton.setPos(sf::Vector2f(700, 100));


    this->reset();
}

void HubHandler::update(float dt, sf::Vector2f mousePos)
{
    switch (this->state)
    {
    case State::main:
        updateMain(mousePos);
        break;

    case State::inventory:
        if (backButton.getBounds().contains(mousePos))
        {
            this->backButton.setTexture(TextureHandler::get().getTexture(ON_TEX));
            if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left))
            {
                state = State::main;
                ui->getInventory()->closeInventory();
            }
        }

        else
            this->backButton.setTexture(TextureHandler::get().getTexture(OFF_TEX));

        break;
    default:
        break;
    }

}

void HubHandler::reset()
{
    this->levelSelected = -1;
    this->state = State::main;
    ui->getInventory()->setQuickslotHidden(true);
}

void HubHandler::updateMain(sf::Vector2f mousePos)
{
    for (int i = 0; i < MAIN_BUTTON_COUNT; i++)
    {
        if (mainButtons[i].getBounds().contains(mousePos))
        {
            this->mainButtons[i].setTexture(TextureHandler::get().getTexture(ON_TEX));
            if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left))
            {
                switch (i)
                {
                case 0:
                    levelSelected = 0;
                    ui->getInventory()->setQuickslotHidden(false);
                    break;

                case 1:
                    this->state = State::inventory;
                    ui->getInventory()->openInventory();
                    break;
                default:
                    break;
                }
            }
        }

        else
            this->mainButtons[i].setTexture(TextureHandler::get().getTexture(OFF_TEX));

    }

}

void HubHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(background, states);

    switch (state)
    {
    case HubHandler::State::main:
        for (int i = 0; i < MAIN_BUTTON_COUNT; i++)
            target.draw(mainButtons[i], states);
        break;
    case HubHandler::State::level:
        break;
    case HubHandler::State::inventory:
        target.draw(backButton, states);
        break;
    default:
        break;
    }



}
