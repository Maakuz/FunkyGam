#include "HubHandler.h"
#include "Game/Handlers/TextureHandler.h"
#include "Game/Misc/MouseState.h"
#include "Game/Misc/Definitions.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Handlers/ItemHandler.h"

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
    this->selectedLevel = -1;
    this->ui = uiHandler;
    this->background.setTexture(*TextureHandler::get().getTexture(6), true);
    this->background.setScale(2, 2);
    this->alchemist.setTexture(*TextureHandler::get().getTexture(14), true);
    this->adventurer.setTexture(*TextureHandler::get().getTexture(18), true);

    this->loadInterface();

    this->reset();

    this->loadRecipes();
    this->loadLevelInfo();
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

    case State::level:
        updateLevelSelect(mousePos);
        updateBack(mousePos, State::main);
        break;

    case State::alchemist:
        updateAlchemy(mousePos);
        updateBack(mousePos, State::main);
        break;
    default:
        break;
    }

}

const LevelInfo* HubHandler::getActiveLevel() const 
{ 
    if (activeLevel == -1)
        return nullptr;

    return &levels[activeLevel]; 
}

const LevelInfo* HubHandler::changeToNextLevel(int exitTaken)
{
    this->activeLevel = levels[activeLevel].unlockLevels[exitTaken];

    if (activeLevel == -1)
        return nullptr;

    return &levels[activeLevel];
}

void HubHandler::reset()
{
    if (selectedLevel != -1)
        this->levels[selectedLevel].name.setOutlineThickness(0);

    if (selectedRecipe != -1)
        this->recipes[selectedRecipe].name.setOutlineThickness(0);

    this->infoBox.setText("");
    this->descriptionBox.setText("");

    this->activeLevel = -1;
    this->state = State::main;
    this->selectedRecipe = -1;
    this->selectedLevel = -1;
    ui->getInventory()->setQuickslotHidden(true);

    const std::unordered_set<int>* foundItems = ItemHandler::getFoundItems();

    for (Recipe& recipe : recipes)
    {
        if (!recipe.unlocked)
        {
            for (const int ingredient : recipe.components)
                if (foundItems->count(ingredient))
                    recipe.unlocked = true;
                
        }
    }
    
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
        file >> trash >> recipe.resultAmount;
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

void HubHandler::loadLevelInfo()
{
    this->levels.clear();

    std::ifstream file(DATA_PATH "LevelInfo.mop");

    if (!file.is_open())
    {
        printf("Could not open \"LevelInfo.mop\". Terminating computer hehe\n");
        system("pause");
        exit(-2);
    }

    std::string trash;
    int levelCount;

    file >> trash;
    file >> trash >> levelCount;

    for (int i = 0; i < levelCount; i++)
    {
        LevelInfo info;
        file >> info;

        info.setCharacterSize(this->infoTextCharacterSize);
        info.setFillColor(this->textFillColor);
        info.setOutlineColor(this->textOutlineColor);
        levels.push_back(info);
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
    //General
    sf::Vector2f spacing;
    int textureID;
    int hoverTextureID;
    int inactiveTextureID;

    //Text
    int textWindowTextureID;
    int characterSize;
    int philCollins[3];
    int outlineColor[3];

    //Main buttons
    std::string trash;
    sf::Vector2f pos;
    sf::Vector2f size;
    int buttonCharacterSize;

    int mainButtonCount;
    std::vector<std::string> labels;
    
    //Back button
    sf::Vector2f backPos;
    sf::Vector2f backSize;

    //Alchemy
    sf::Vector2f recipeListPos;
    sf::Vector2f recipeListSize;
    int recipeDescWidth;
    sf::Vector2f craftPos;
    sf::Vector2f craftSize;

    //General read
    file >> trash;
    file >> trash >> spacing.x >> spacing.y;
    file >> trash >> textWindowTextureID;
    file >> trash >> textureID;
    file >> trash >> hoverTextureID;
    file >> trash >> inactiveTextureID;

    //Text read
    file >> trash;
    file >> trash >> characterSize;
    file >> trash >> philCollins[0] >> philCollins[1] >> philCollins[2];
    file >> trash >> outlineColor[0] >> outlineColor[1] >> outlineColor[2];

    //Main button read
    file >> trash;
    file >> trash >> pos.x >> pos.y;
    file >> trash >> size.x >> size.y;
    file >> trash >> buttonCharacterSize;
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
    file >> trash >> recipeDescWidth;
    file >> trash >> craftPos.x >> craftPos.y;
    file >> trash >> craftSize.x >> craftSize.y;

    file.close();


    this->textureOff = TextureHandler::get().getTexture(textureID);
    this->textureOn = TextureHandler::get().getTexture(hoverTextureID);
    this->textureTextWindow = TextureHandler::get().getTexture(textWindowTextureID);
    this->textureInactive = TextureHandler::get().getTexture(inactiveTextureID);

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

    this->listBackground.create(this->textureTextWindow, TextureHandler::get().getFont());
    this->listBackground.setPos(recipeListPos);
    this->listBackground.setWidth(recipeListSize.x);
    this->listBackground.setHeight(recipeListSize.y);

    this->descriptionBox.create(this->textureTextWindow, TextureHandler::get().getFont());
    this->descriptionBox.setWidth(recipeDescWidth);
    this->descriptionBox.setHeight(listBackground.getBounds().height - this->alchemist.getGlobalBounds().height - spacing.y);
    sf::Vector2f descPos = listBackground.getPos();
    descPos.x += listBackground.getBounds().width + spacing.x;
    descPos.y += listBackground.getBounds().height - descriptionBox.getBounds().height;
    this->descriptionBox.setPos(descPos);

    this->infoBox.create(this->textureTextWindow, TextureHandler::get().getFont());
    this->infoBox.setCharacterSize(characterSize);
    this->infoBox.setWidth(recipeDescWidth - this->alchemist.getGlobalBounds().width - spacing.x);
    this->infoBox.setHeight(this->alchemist.getGlobalBounds().height);
    this->infoBox.setPos(sf::Vector2f(recipeListPos.x + recipeListSize.x + spacing.x, recipeListPos.y));

    this->acceptButton.create(this->textureInactive, TextureHandler::get().getFont());
    this->acceptButton.setCharacterSize(buttonCharacterSize);
    this->acceptButton.setText("Craft");
    this->acceptButton.setPos(craftPos);
    this->acceptButton.setWidth(craftSize.x);
    this->acceptButton.setHeight(craftSize.y);
    
    float alchemyX = recipeListPos.x + recipeListSize.x + spacing.x + recipeDescWidth - this->alchemist.getGlobalBounds().width;
    this->alchemist.setPosition(alchemyX, recipeListPos.y);
    this->adventurer.setPosition(alchemyX, recipeListPos.y);
}

void HubHandler::updateBack(sf::Vector2f mousePos, State backstate)
{
    if (backButton.getBounds().contains(mousePos))
    {
        this->backButton.setTexture(this->textureOn);
        if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left))
        {
            state = backstate;
            infoBox.setText("");
            descriptionBox.setText("");
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
                    this->state = State::level;
                    this->acceptButton.setText("Embark!");
                    break;

                case 1:
                    this->state = State::inventory;
                    ui->getInventory()->openInventory();
                    break;

                case 2:
                    this->state = State::alchemist;
                    this->acceptButton.setText("Trade");
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

void HubHandler::updateLevelSelect(sf::Vector2f mousePos)
{
    for (int i = 0; i < this->levels.size(); i++)
    {
        LevelInfo* info = &this->levels[i];

        sf::Vector2f pos = this->listBackground.getTextPos() + sf::Vector2f(0, i * DEFAULT_TEXT_SIZE);
        info->name.setPosition(pos);
        info->description.setPosition(this->descriptionBox.getTextPos());

        if (info->name.getGlobalBounds().contains(mousePos) && MOUSE::MouseState::isButtonClicked(sf::Mouse::Left))
        {
            info->name.setOutlineThickness(1);

            if (this->selectedLevel != -1 && this->selectedLevel != i)
                this->levels[this->selectedLevel].name.setOutlineThickness(0);
            this->selectedLevel = i;
        }
    }

    if (selectedLevel != -1)
    {
        this->infoBox.setText("Emenies:\nyes");


        this->infoBox.appendText("\n\nPossible items:\nitems");

        if (acceptButton.getBounds().contains(mousePos))
        {
            acceptButton.setTexture(textureOn);

            if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left))
            {
                activeLevel = levels[selectedLevel].levelID;
                ui->getInventory()->setQuickslotHidden(false);
            }

        }
        else
            acceptButton.setTexture(textureOff);
    }

    else
        acceptButton.setTexture(textureInactive);



}

void HubHandler::updateAlchemy(sf::Vector2f mousePos)
{
    int i = 0;
    for (Recipe& recipe : recipes)
    {
        if (recipe.unlocked)
        {
            sf::Vector2f pos = this->listBackground.getTextPos() + sf::Vector2f(0, i * DEFAULT_TEXT_SIZE);
            recipe.name.setPosition(pos);
            recipe.description.setPosition(this->descriptionBox.getTextPos());

            if (recipe.name.getGlobalBounds().contains(mousePos) && MOUSE::MouseState::isButtonClicked(sf::Mouse::Left))
            {
                recipe.name.setOutlineThickness(1);

                if (this->selectedRecipe != -1 && this->selectedRecipe != i)
                    this->recipes[this->selectedRecipe].name.setOutlineThickness(0);
                this->selectedRecipe = i;
            }
            i++;
        }
    }


    if (selectedRecipe != -1)
    {
        Recipe* recipe = &this->recipes[this->selectedRecipe];
        bool craftingClearance = true;

        this->infoBox.setText("Requires:\n");

        for (int i = 0; i < recipe->components.size(); i++)
        {
            this->infoBox.appendText(std::to_string(recipe->componentAmounts[i]) + " ");
            this->infoBox.appendText(ItemHandler::getTemplate(recipe->components[i])->getName() + "\n");
        }

        this->infoBox.appendText("\n\nYou have:\n");

        for (int i = 0; i < recipe->components.size(); i++)
        {
            int currentAmount = ui->getInventory()->countItem(recipe->components[i]);
            if (currentAmount < recipe->componentAmounts[i])
                craftingClearance = false;


            this->infoBox.appendText(std::to_string(currentAmount) + " ");
            this->infoBox.appendText(ItemHandler::getTemplate(recipe->components[i])->getName() + "\n");
        }

        if (craftingClearance)
        {
            if (acceptButton.getBounds().contains(mousePos))
            {
                acceptButton.setTexture(textureOn);

                if (MOUSE::MouseState::isButtonClicked(sf::Mouse::Left))
                {
                    for (int i = 0; i < recipe->components.size(); i++)
                    {
                        this->ui->getInventory()->removeItem(recipe->components[i], recipe->componentAmounts[i]);
                    }

                    this->ui->getInventory()->addItem(recipe->resultID, recipe->resultAmount);
                    this->ui->getInventory()->sortItems();
                }

            }


            else
                acceptButton.setTexture(textureOff);
        }

        else
            acceptButton.setTexture(textureInactive);
    }

    else
        acceptButton.setTexture(textureInactive);

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

    case HubHandler::State::inventory:
        target.draw(backButton, states);
        break;

    case HubHandler::State::alchemist:
        target.draw(backButton, states);
        target.draw(acceptButton, states);
        target.draw(listBackground, states);
        target.draw(descriptionBox, states);
        target.draw(infoBox, states);
        target.draw(alchemist, states);
        for (const Recipe& recipe : this->recipes)
            if (recipe.unlocked)
                target.draw(recipe.name, states);
        
        if (this->selectedRecipe != -1)
            target.draw(this->recipes[this->selectedRecipe].description, states);
        break;

    case HubHandler::State::level:
        target.draw(backButton, states);
        target.draw(acceptButton, states);
        target.draw(listBackground, states);
        target.draw(descriptionBox, states);
        target.draw(infoBox, states);
        target.draw(adventurer, states);
        for (const LevelInfo& level : this->levels)
            target.draw(level.name, states);

        if (this->selectedLevel != -1)
            target.draw(this->levels[this->selectedLevel].description, states);
        break;
    default:
        break;
    }



}

