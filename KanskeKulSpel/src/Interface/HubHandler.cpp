#include "HubHandler.h"
#include "Handlers/TextureHandler.h"
#include "Misc/MouseState.h"
#include "Misc/Definitions.h"
#include "Misc/ConsoleWindow.h"

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
    ConsoleWindow::get().addCommand("reloadHubInterface", [&](Arguments args)->std::string 
        {

            loadInterface();
            for (int i = 0; i < recipes.size(); i++)
            {
                recipes[i].setCharacterSize(infoTextCharacterSize);
                recipes[i].setFillColor(textFillColor);
                recipes[i].setOutlineColor(textOutlineColor);
            }

            return "Locked and maybe loaded";
        });

    this->infoTextCharacterSize = DEFAULT_TEXT_SIZE;
    this->selectedRecipe = -1;
    this->ui = uiHandler;
    this->background.setTexture(*TextureHandler::get().getTexture(6), true);
    this->background.setScale(2, 2);

    this->loadInterface();

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
    this->selectedRecipe = -1;
    ui->getInventory()->setQuickslotHidden(true);
}

void HubHandler::loadRecipes()
{
    this->recipes.clear();

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

        recipe.setCharacterSize(this->infoTextCharacterSize);
        recipe.setFillColor(this->textFillColor);
        recipe.setOutlineColor(this->textOutlineColor);
        recipes.push_back(recipe);
    }

    file.close();
}

void HubHandler::loadInterface()
{
    mainButtons.clear();

    std::ifstream file(DATA_PATH "HubInterface.mop");

    if (!file.is_open())
    {
        printf("Could not open \"HubInterface.mop\". Death\n");
        system("pause");
        exit(-43);
    }
    //Text
    int textWindowTextureID;
    int characterSize;
    int philCollins[3];
    int outlineColor[3];

    //Main buttons
    std::string trash;
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Vector2f spacing;
    int buttonCharacterSize;
    int textureID;
    int hoverTextureID;
    int mainButtonCount;
    std::vector<std::string> labels;
    
    //Back button
    sf::Vector2f backPos;
    sf::Vector2f backSize;

    //Alchemy
    sf::Vector2f recipeListPos;
    sf::Vector2f recipeListSize;

    //Text read
    file >> trash;
    file >> trash >> textWindowTextureID;
    file >> trash >> characterSize;
    file >> trash >> philCollins[0] >> philCollins[1] >> philCollins[2];
    file >> trash >> outlineColor[0] >> outlineColor[1] >> outlineColor[2];

    //Main button read
    file >> trash;
    file >> trash >> pos.x >> pos.y;
    file >> trash >> size.x >> size.y;
    file >> trash >> spacing.x >> spacing.y;
    file >> trash >> buttonCharacterSize;
    file >> trash >> textureID;
    file >> trash >> hoverTextureID;
    file >> trash >> mainButtonCount;

    file >> trash;
    file.ignore();

    for (int i = 0; i < mainButtonCount; i++)
    {
        std::string label;
        std::getline(file, label);
        labels.push_back(label);
    }

    //back button read
    file >> trash;
    file >> trash >> backPos.x >> backPos.y;
    file >> trash >> backSize.x >> backSize.y;

    //Inventory read heh
    file >> trash;

    //Alchemy read
    file >> trash;
    file >> trash >> recipeListPos.x >> recipeListPos.y;
    file >> trash >> recipeListSize.x >> recipeListSize.y;


    file.close();


    this->textureOff = TextureHandler::get().getTexture(textureID);
    this->textureOn = TextureHandler::get().getTexture(hoverTextureID);
    this->textureTextWindow = TextureHandler::get().getTexture(textWindowTextureID);

    this->infoTextCharacterSize = characterSize;
    this->textFillColor = sf::Color(philCollins[0], philCollins[1], philCollins[2]);
    this->textOutlineColor = sf::Color(outlineColor[0], outlineColor[1], outlineColor[2]);

    for (int i = 0; i < mainButtonCount; i++)
    {
        TextBubble button(this->textureOff, TextureHandler::get().getFont());
        button.setCharacterSize(buttonCharacterSize);
        button.setText(labels[i]);
        button.setWidth(size.x);
        button.setHeight(size.y);
        button.setPos(pos + sf::Vector2f(0, (size.y + spacing.y) * i));
        
        mainButtons.push_back(button);
    }

    this->backButton.create(this->textureOff, TextureHandler::get().getFont());
    this->backButton.setCharacterSize(buttonCharacterSize);
    this->backButton.setText("Back");
    this->backButton.setPos(backPos);
    this->backButton.setWidth(backSize.x);
    this->backButton.setHeight(backSize.y);

    this->recipeListBackground.create(this->textureTextWindow, TextureHandler::get().getFont());
    this->recipeListBackground.setPos(recipeListPos);
    this->recipeListBackground.setWidth(recipeListSize.x);
    this->recipeListBackground.setHeight(recipeListSize.y);

}

void HubHandler::updateBack(sf::Vector2f mousePos, State backstate)
{
    if (backButton.getBounds().contains(mousePos))
    {
        this->backButton.setTexture(this->textureOn);
        if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left))
        {
            state = backstate;
            ui->getInventory()->closeInventory();
        }
    }

    else
        this->backButton.setTexture(this->textureOff);

}

void HubHandler::updateMain(sf::Vector2f mousePos)
{
    for (int i = 0; i < mainButtons.size(); i++)
    {
        if (mainButtons[i].getBounds().contains(mousePos))
        {
            this->mainButtons[i].setTexture(this->textureOn);
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
            this->mainButtons[i].setTexture(this->textureOff);

    }

}

void HubHandler::updateAlchemy(sf::Vector2f mousePos)
{
    for (int i = 0; i < this->recipes.size(); i++)
    {
        Recipe* recipe = &this->recipes[i];

        sf::Vector2f pos = this->recipeListBackground.getTextPos() + sf::Vector2f(0, i * DEFAULT_TEXT_SIZE);
        recipe->name.setPosition(pos);

        if (recipe->name.getGlobalBounds().contains(mousePos) && MOUSE::MouseState::isButtonClicked(sf::Mouse::Left))
        {
            recipe->name.setOutlineThickness(1);

            if (this->selectedRecipe != -1)
                this->recipes[this->selectedRecipe].name.setOutlineThickness(0);
            this->selectedRecipe = i;
        }
    }

}

void HubHandler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(background, states);

    switch (state)
    {
    case HubHandler::State::main:
        for (const TextBubble& text : mainButtons)
            target.draw(text, states);
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
