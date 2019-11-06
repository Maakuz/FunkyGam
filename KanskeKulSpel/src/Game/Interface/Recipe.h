#pragma once
#include "Game/Interface/Info.h"

struct Recipe : public Info
{
    std::vector<int> components;
    std::vector<int> componentAmounts;
    int resultID;
    int resultAmount;
    bool oneTimeRecipe;

    Recipe()
    {
        resultID = 0;
        resultAmount = 1;
        oneTimeRecipe = false;
    }

    friend std::istream& operator>>(std::istream& in, Recipe& recipe);
    friend std::ostream& operator<<(std::ostream& out, const Recipe& recipe);

};