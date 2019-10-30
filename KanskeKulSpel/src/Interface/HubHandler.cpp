#include "HubHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/MouseState.h"
#include "Misc/Definitions.h"

#define OFF_TEX 8
#define ON_TEX 9


HubHandler::Recipe::Recipe()
{
    resultID = -1;
    unlocked = false;
    seen = false;
    name.setFont(*TextureHandler::get().getFont());
    name.setCharacterSize(DEFAULT_TEXT_SIZE);
    name.setFillColor(sf::Color::Black);
    description.setFont(*TextureHandler::get().getFont());
    description.setCharacterSize(DEFAULT_TEXT_SIZE);
    description.setFillColor(sf::Color::Black);
}


HubHandler::HubHandler(UIHandler* uiHandler)
{
    this->ui = uiHandler;
    this->background.setTexture(*TextureHandler::get().getTexture(6), true);
    this->background.setScale(2, 2);
    std::string mainButtonText[MAIN_BUTTON_COUNT] = {"Play level 1", "Manage inventory", "Visit alchemist"};

    for (int i = 0; i < MAIN_BUTTON_COUNT; i++)
    {
        this->mainButtons[i].create(TextureHandler::get().getTexture(8), TextureHandler::get().getFont());
        this->mainButtons[i].setCharacterSize(16);
        this->mainButtons[i].setText(mainButtonText[i]);
        this->mainButtons[i].resizeToFit();
        this->mainButtons[i].setWidth(150);
        this->mainButtons[i].setPos(sf::Vector2f(600, 100 + i * 100));
    }

    this->backButton.create(TextureHandler::get().getTexture(8), TextureHandler::get().getFont());
    this->backButton.setCharacterSize(16);
    this->backButton.setText("Back");
    this->backButton.resizeToFit();
    this->backButton.setPos(sf::Vector2f(700, 100));

    this->recipeListBackground.create(TextureHandler::get().getTexture(7), TextureHandler::get().getFont());
    this->recipeListBackground.setPos(sf::Vector2f(800, 100));
    this->recipeListBackground.setWidth(300);
    this->recipeListBackground.setHeight(500);

    this->reset();

    this->loadRecipes();
}

void HubHandler::update(float dt, sf::Vector2f mousePos)
{
    switch (this->state)
    {
    case State::main:
        updateMain(mousePos);
        break;

    case State::inventory:
        updateBack(mousePos, State::main);
        break;

    case State::alchemist:
        updateBack(mousePos, State::main);
        updateAlchemy(mousePos);
        break;
    default:
        break;
    }

}

void HubHandler::reset()
{
    this->levelSelected = Level::none;
    this->state = State::main;
    ui->getInventory()->setQuickslotHidden(true);
}

void HubHandler::loadRecipes()
{
    std::ifstream file(DATA_PATH "Recipes.mop");

    if (!file.is_open())
    {
        printf("Could not open \"Recipes.mop\". Terminating computer hehe\n");
        system("pause");
        exit(-2);
    }

    std::string trash;
    int recipeCount;
    
    file >> trash;
    file >> trash >> recipeCount;

    for (int i = 0; i < recipeCount; i++)
    {
        Recipe recipe;
        std::string name;
        std::string desc;
        std::string descLine;
        int componentCount;

        file >> trash;
        file >> trash;
        std::getline(file, name);
        while (name.size() > 0 && name[0] == ' ')
            name.erase(name.begin());

        file >> trash >> componentCount;

        file >> trash;
        for (int i = 0; i < componentCount; i++)
        {
            int componentID;
            file >> componentID;
            recipe.components.push_back(componentID);
        }

        file >> trash;
        for (int i = 0; i < componentCount; i++)
        {
            int componentAmount;
            file >> componentAmount;
            recipe.componentAmounts.push_back(componentAmount);
        }

        file >> trash >> recipe.resultID;
        file >> trash;

        file.ignore();
        std::getline(file, descLine);
        while (descLine != "[DescriptionEnd]")
        {
            desc.append(descLine + "\n");
            std::getline(file, descLine);
        }

        recipe.name.setString(name);
        recipe.description.setString(desc);

        recipes.push_back(recipe);
    }

    file.close();
}

void HubHandler::updateBack(sf::Vector2f mousePos, State backstate)
{
    if (backButton.getBounds().contains(mousePos))
    {
        this->backButton.setTexture(TextureHandler::get().getTexture(ON_TEX));
        if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left))
        {
            state = backstate;
            ui->getInventory()->closeInventory();
        }
    }

    else
        this->backButton.setTexture(TextureHandler::get().getTexture(OFF_TEX));

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
                    levelSelected = Level::forest;
                    ui->getInventory()->setQuickslotHidden(false);
                    break;

                case 1:
                    this->state = State::inventory;
                    ui->getInventory()->openInventory();
                    break;

                case 2:
                    this->state = State::alchemist;
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

void HubHandler::updateAlchemy(sf::Vector2f mousePos)
{
    for (int i = 0; i < this->recipes.size(); i++)
    {
        Recipe* recipe = &this->recipes[i];

        sf::Vector2f pos = this->recipeListBackground.getTextPos() + sf::Vector2f(0, i * DEFAULT_TEXT_SIZE);
        recipe->name.setPosition(pos);
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

    case HubHandler::State::alchemist:
        target.draw(backButton, states);
        target.draw(recipeListBackground, states);
        for (const Recipe& recipe : this->recipes)
        {
            target.draw(recipe.name, states);
        }
        break;
    default:
        break;
    }



}
