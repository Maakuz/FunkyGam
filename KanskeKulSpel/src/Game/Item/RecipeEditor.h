#pragma once
#include <vector>
#include "Game/Interface/Recipe.h"
class RecipeEditor
{
public:
    RecipeEditor();
    ~RecipeEditor();

    void openWindow();
    void closeWindow() { open = false; };
    bool isOpen() const { return open; };
    void update();
private:
    bool open;

    struct EditorRecipe 
    {
        Recipe recipe;
        std::string name;
        std::string desc;
    };
    std::vector<EditorRecipe> recipes;

    int currentItem;

    void showRecipeData(EditorRecipe& recipe);
    void readItems();
    void writeRecipes();
};