#pragma once
#include "Game/Interface/Info.h"

struct Recipe : public Info
{
    std::vector<int> components;
    std::vector<int> componentAmounts;
    int resultID;
    int resultAmount;

    Recipe()
    {
        resultID = -1;
        resultAmount = 1;
    }

    friend std::istream& operator>>(std::istream& in, Recipe& recipe);
};