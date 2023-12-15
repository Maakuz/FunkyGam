#pragma once
#include "Misc/GlobalDefinitions.h"

#define DEFAULT_WORK_AREA_X 20
#define DEFAULT_WORK_AREA_Y 20

#define RESOURCE_FOLDER "../Resources/"

#define TILE_MAP_FOLDER RESOURCE_FOLDER "Maps/Textures/"
#define TILE_MAP_PATH(x) TILE_MAP_FOLDER x

#define DEFAULT_SAVE_PATH "../Resources/Maps/"
#define DEFAULT_QUICKSAVE_FILE_NAME "QuackSwack"
#define FILE_EXTENSION ".yay"

#define TILEMENU_X_AREA 5
#define TILEMENU_Y_AREA 20
#define TILEMENU_BORDER_SIZE 8
#define TILEMENU_WIDTH ((TILE_SIZE * TILEMENU_X_AREA) + (TILEMENU_BORDER_SIZE * 2))

#define COORDINATE_BAR_SIZE 20

#define TILE_LAYER_AMOUNT 3
#define LAYER_AMOUNT 4
static const char* LAYER_BUTTON_LABELS[LAYER_AMOUNT] = { "Layer 1", "Layer 2", "Layer 3", "Hitbox layer" };

#define HITBOX_ID_START 500
#define NR_OF_HITBOXES 9
#define HITBOX_LAYER 3
#define SPECIAL_HITBOX 999

#define MENU_BAR_HEIGHT 20

#define MAXIMUM_WORKSPACE 10000