#include "RecipeEditor.h"
#include <fstream>
#include "Game/Misc/Definitions.h"
#include "Misc/ConsoleWindow.h"
#include "Game/Item/ItemHandler.h"

RecipeEditor::RecipeEditor()
{
    open = false;
    currentItem = -1;
}

RecipeEditor::~RecipeEditor()
{
}

void RecipeEditor::openWindow()
{
    open = true;
    readItems();
}

void RecipeEditor::update()
{
    ImGui::Begin("Recipe Editor", &this->open);
    if (!recipes.empty())
    {
        if (ImGui::BeginCombo("Select item", this->recipes[this->currentItem].name.c_str()))
        {
            for (int i = 0; i < recipes.size(); i++)
            {
                if (ImGui::Selectable(recipes[i].name.c_str()))
                    this->currentItem = i;
            }


            ImGui::EndCombo();
        }

        showRecipeData(recipes[currentItem]);
    }

    if (ImGui::Button("save"))
        writeRecipes();

    if (ImGui::Button("Create new Recipe"))
    {
        EditorRecipe recipe;
        recipe.recipe.resultID = 0;
        recipe.name = "New Recipe";
        recipe.desc = "This is a default description\n";
        this->recipes.push_back(recipe);
        this->currentItem = this->recipes.size() - 1;
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete"))
    {
        this->recipes.erase(recipes.begin() + currentItem);
        this->currentItem = this->recipes.size() - 1;
    }
    ImGui::End();
}

void RecipeEditor::showRecipeData(EditorRecipe& recipe)
{
    ImGui::Separator();

    ImGui::InputText("Name", &recipe.name);
    ImGui::PushItemWidth(200);

    const std::vector<const Item*>* items = ItemHandler::getTemplateVec();
    for (int i = 0; i < recipe.recipe.components.size(); i++)
    {
        std::string compL = "Component #" + std::to_string(i);
        
        if (ImGui::BeginCombo(compL.c_str(), items->at(recipe.recipe.components[i])->getName().c_str()))
        {
            for (int j = 0; j < items->size(); j++)
            {
                if (ImGui::Selectable(items->at(j)->getName().c_str()))
                    recipe.recipe.components[i] = j;
            }


            ImGui::EndCombo();
        }

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(("ID: " + std::to_string(items->at(recipe.recipe.components[i])->getID())).c_str());

        ImGui::SameLine();
        std::string compAmountL = "Amount #" + std::to_string(i);
        ImGui::InputInt(compAmountL.c_str(), &recipe.recipe.componentAmounts[i]);
    }

    if (ImGui::Button("Add component"))
    {
        recipe.recipe.components.push_back(0);
        recipe.recipe.componentAmounts.push_back(1);
    }
    ImGui::SameLine();

    if (ImGui::Button("Delete last"))
    {
        recipe.recipe.components.pop_back();
        recipe.recipe.componentAmounts.pop_back();
    }

    ImGui::PopItemWidth();   
    ImGui::Separator();

    if (ImGui::BeginCombo("Item created", items->at(recipe.recipe.resultID)->getName().c_str()))
    {
        for (int i = 0; i < items->size(); i++)
        {
            if (ImGui::Selectable(items->at(i)->getName().c_str()))
                recipe.recipe.resultID = i;
        }


        ImGui::EndCombo();
    }

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip(("ID: " + std::to_string(items->at(recipe.recipe.resultID)->getID())).c_str());

    ImGui::InputInt("Items created", &recipe.recipe.resultAmount);

    ImGui::InputTextMultiline("Description", &recipe.desc);

    ImGui::Separator();
}

void RecipeEditor::readItems()
{
    this->recipes.clear();

    std::ifstream file(DATA_PATH "Recipes.mop");

    if (!file.is_open())
    {
        printCon("Could not open \"Recipes.mop\". Terminating computer hehe jk lol");
    }

    std::string trash;
    int recipeCount;

    file >> trash;
    file >> trash >> recipeCount;

    for (int i = 0; i < recipeCount; i++)
    {
        EditorRecipe recipe;
        file >> recipe.recipe;
        recipe.name = recipe.recipe.name.getString();
        recipe.desc = recipe.recipe.description.getString();

        recipes.push_back(recipe);
    }

    this->currentItem = recipes.size() - 1;
    file.close();
}

void RecipeEditor::writeRecipes()
{
    std::ofstream file(DATA_PATH "Recipes.mop");

    if (!file.is_open())
    {
        printCon("File not found, editing unavailible");
        closeWindow();
    }

    file << "[Recipes]\n" << "RecipeCount: " << recipes.size();

    for (EditorRecipe recipe : recipes)
    {
        recipe.recipe.name.setString(recipe.name);
        recipe.recipe.description.setString(recipe.desc);
        file << "\n" << "\n" << recipe.recipe;
    }

    file.close();
}