#pragma once
#include "Game/Particles/Emitter.h"
#include "Game/Item/Item.h"

struct GatherItem
{
    Emitter* emitter;
    Entity item;
    int count;
};