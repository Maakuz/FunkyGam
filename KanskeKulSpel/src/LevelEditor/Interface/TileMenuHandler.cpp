#include "TileMenuHandler.h"
#include "LevelEditor/Queues/TileQueue.h"
#include "LevelEditor/Queues/OverlayQueue.h"
#include "LevelEditor/Queues/BackgroundQueue.h"
#include "LevelEditor/Queues/ToolTileQueue.h"
#include "Misc/Structs.h"
#include "LevelEditor/Constants.h"
#include "SFML/Window/Mouse.hpp"
#include "Imgui/imgui.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"
#include "LevelEditor/TileMaps.h"



TileMenuHandler::TileMenuHandler() :
    tileBox(TILEMENU_BORDER_SIZE, TILEMENU_BORDER_SIZE + MENU_BAR_HEIGHT, TILE_SIZE* TILEMENU_X_AREA, TILE_SIZE* TILEMENU_Y_AREA, TILEMENU_BORDER_SIZE)
{
    this->tileBox.setColor(sf::Color(0, 100, 200));
    this->tileBox.setBorderColor(sf::Color(100, 100, 100));
    this->activeTileTexture = 0;
    this->offset.x = TILEMENU_BORDER_SIZE;
    this->offset.y = TILEMENU_BORDER_SIZE + MENU_BAR_HEIGHT;

    this->selectingBlocks = false;
    this->rightClicking = false;
    this->isImportingTexture = false;
    this->renderingLights = false;
    this->gridVisible = false;
    this->turboHitboxOverride = false;
    this->overlayInToolBox = false;
    this->activeTileTexture = -1;

    this->createTileButtons();
    this->infoWindow.fetchInfo("");
}

void TileMenuHandler::handleEvent(sf::Event event, bool guiBlock, sf::Vector2i viewPortMousePos)
{
    this->handleMouseEvents(event, guiBlock, viewPortMousePos);

    this->handleKeyboardEvents(event);
}

void TileMenuHandler::handleMouseEvents(sf::Event event, bool guiBlock, sf::Vector2i viewPortMousePos)
{
    switch (event.type)
    {
    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left && !rightClicking &&
            !guiBlock && !lightManager.isPlacingLight() && !turboHitboxOverride)
        {
            pressedPos = { event.mouseButton.x, event.mouseButton.y };

            if (tileBox.contains(pressedPos.x, pressedPos.y))
            {
                selectingBlocks = true;
                overlayInToolBox = true;
            }
        }

        if (event.mouseButton.button == sf::Mouse::Right && !selectingBlocks && 
            !guiBlock && !lightManager.isPlacingLight())
        {
            if (!tileBox.contains(event.mouseButton.x, event.mouseButton.y))
            {
                pressedPos = viewPortMousePos;

                rightClicking = true;
                overlayInToolBox = false;
            }
        }
        break;


    case sf::Event::MouseButtonReleased:

        if (event.mouseButton.button == sf::Mouse::Left)
        {
            if (selectingBlocks)
            {
                selectingBlocks = false;
            }
        }

        if (event.mouseButton.button == sf::Mouse::Right)
        {
            if (!selectingBlocks && turboHitboxOverride)
                releasedPos = viewPortMousePos;

            if (rightClicking)
                rightClicking = false;
        }
        break;
    }
}

void TileMenuHandler::handleKeyboardEvents(sf::Event event)
{
    if (turboHitboxOverride)
        return;

    switch (event.type)
    {
    case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::S && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            saveAsCurrentFile();

        if (event.key.code == sf::Keyboard::Z && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            layerManager.undo();

        if (event.key.code == sf::Keyboard::Y && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            layerManager.redo();

        if (event.key.code == sf::Keyboard::C && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            copiedTiles = activeTiles;

        if (event.key.code == sf::Keyboard::X && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        {
            copiedTiles = activeTiles;

            std::vector<ActiveTile> eraseTiles = activeTiles;

            for (size_t i = 0; i < eraseTiles.size(); i++)
            {
                eraseTiles[i].tileID = -1;
            }

            layerManager.insertTiles(eraseTiles, pressedPos);
            activeTiles.clear();
        }

        if (event.key.code == sf::Keyboard::V && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            layerManager.insertTiles(copiedTiles, pressedPos);

        if (event.key.code == sf::Keyboard::E)
            equipEraser();

        //Overcomplicated but saved three lines
        for (int i = 0; i < LAYER_AMOUNT; i++)
        {
            if (event.key.code == i + sf::Keyboard::Num1)
            {
                layerManager.setActiveLayer(i);
            }
        }

        if (event.key.code == sf::Keyboard::F5)
            fileManager.quickSave(this->layerManager, this->lightManager, hitbaxes);

        break;
    }
}

void TileMenuHandler::update(sf::Vector2i mousePos, sf::Vector2i viewPortOffset, bool guiActive)
{
    this->handleMenuBar();
    this->handleHelpWindow();
    if (saveWindow.update())
        saveFile();

    if (loadWindow.update())
        loadFile();

    lightManager.update(viewPortOffset, guiActive);
    lightManager.setDefaultLightPos((sf::Vector2f)viewPortOffset);
    lightManager.queueLights();

    if (lightManager.isPlacingLight())
        activeTiles.clear();

    if (guiActive || lightManager.isPlacingLight())
        return;


    if (!selectingBlocks)
    {
        if (rightClicking)
        {
            handleLayerSelection(pressedPos, viewPortOffset);
        }

        else if (!turboHitboxOverride)
            layerManager.update(activeTiles, viewPortOffset);
    }

    else
        handleBlockSelection(pressedPos - offset, mousePos - offset);
}

void TileMenuHandler::queueItems(sf::View viewArea)
{
    layerManager.queueTiles(viewArea);


    //toolbox
    if (layerManager.getActiveLayer() == LAYER_AMOUNT - 1)
    {
        for (int i = 0; i < NR_OF_HITBOXES + 1; i++)
        {
            Tile activeTile;
            activeTile.textureID = -1;
            activeTile.tileID = HITBOX_ID_START + i;
            activeTile.x = (i % TILEMENU_X_AREA) * TILE_SIZE + offset.x;
            activeTile.y = (i / TILEMENU_X_AREA) * TILE_SIZE + offset.y;
            ToolTileQueue::get().queue(activeTile);
        }
    }

    else if (activeTileTexture != -1)
    {
        sf::Vector2i bounds = TileMaps::get().getSheetSize(activeTileTexture);

        for (int i = 0; i < TILEMENU_Y_AREA; i++)
        {
            for (int j = 0; j < TILEMENU_X_AREA; j++)
            {
                if (j + TILEMENU_X_AREA * i < bounds.x * bounds.y)
                {
                    Tile activeTile;
                    activeTile.textureID = activeTileTexture;
                    activeTile.tileID = j + TILEMENU_X_AREA * i;
                    activeTile.x = j * TILE_SIZE + offset.x;
                    activeTile.y = i * TILE_SIZE + offset.y;

                    ToolTileQueue::get().queue(activeTile);
                }
            }
        }
    }

    BackgroundQueue::get().queue(tileBox.getRect());



    for (ActiveTile& i : activeTiles)
    {
        std::vector<sf::RectangleShape> rects = i.box;

        for (sf::RectangleShape& rect : rects)
        {
            OverlayQueue::get().queue(rect);
        }
    }
}

void TileMenuHandler::createTileButtons()
{
    buttons.resize(TILEMENU_Y_AREA * TILEMENU_X_AREA);

    for (int i = 0; i < TILEMENU_Y_AREA; i++)
    {
        for (int j = 0; j < TILEMENU_X_AREA; j++)
        {
            buttons[j + TILEMENU_X_AREA * i].setSize(TILE_SIZE, TILE_SIZE);
            buttons[j + TILEMENU_X_AREA * i].setPosition(float(j * TILE_SIZE + offset.x), float(i * TILE_SIZE + offset.y));
        }
    }
}

void TileMenuHandler::handleMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
            {
                layerManager.startOver();
                infoWindow.fetchInfo("");
                activeTiles.clear();
            }

            if (ImGui::MenuItem("Save"))
            {
                saveWindow.openWindow();
            }

            if (ImGui::MenuItem("Open"))
            {
                loadWindow.openWindow();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo"))
            {
                layerManager.undo();
            }

            if (ImGui::MenuItem("Redo"))
            {
                layerManager.redo();
            }

            if (ImGui::MenuItem("Import"))
            {
                isImportingTexture = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Layer"))
        {
            if (ImGui::MenuItem("Layer1"))
            {
                layerManager.setActiveLayer(0);
            }

            if (ImGui::MenuItem("Layer2"))
            {
                layerManager.setActiveLayer(1);
            }

            if (ImGui::MenuItem("Layer3"))
            {
                layerManager.setActiveLayer(2);
            }

            if (ImGui::MenuItem("Hitbox layer"))
            {
                layerManager.setActiveLayer(HITBOX_LAYER);
            }



            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {

            if (ImGui::MenuItem("Layer diff"))
            {
                layerManager.setHighlightLayers(!layerManager.getHighlightLayers());
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Export"))
                fileManager.exportTextures(layerManager);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }



}

void TileMenuHandler::handleHelpWindow()
{
    static const sf::Color ACTIVE_COLOR = sf::Color(100, 200, 200);
    static const sf::Color INACTIVE_COLOR = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    static float workArea[2] = {DEFAULT_WORK_AREA_X, DEFAULT_WORK_AREA_Y};
    ImGui::Begin("Halp");
    if (ImGui::BeginTabBar("tabbu"))
    {
        if (ImGui::BeginTabItem("Textures"))
        {
            if (ImGui::Button("Import tilemap"))
            {
                isImportingTexture = true;
            }


            std::string previewText = "None selected";
            if (activeTileTexture != -1)
                previewText = TileMaps::get().getTextureNameList()[activeTileTexture];

            if (ImGui::BeginCombo("Active texture", previewText.c_str()))
            {
                for (const std::string& name : TileMaps::get().getTextureNameList())
                {
                    if (ImGui::Selectable(name.c_str()))
                        activeTileTexture = TileMaps::get().getTextureIndex(name);
                }
            
                ImGui::EndCombo();
            }


            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Layers"))
        {
            for (int i = 0; i < LAYER_AMOUNT; i++)
            {
                if (i == layerManager.getActiveLayer())
                    ImGui::PushStyleColor(ImGuiCol_Button, ACTIVE_COLOR);

                else
                    ImGui::PushStyleColor(ImGuiCol_Button, INACTIVE_COLOR);

                if (ImGui::Button(LAYER_BUTTON_LABELS[i]))
                {
                    layerManager.setActiveLayer(i);
                }

                ImGui::PopStyleColor();
            }

            bool layerDiffCheck = layerManager.getHighlightLayers();
            if (ImGui::Checkbox("Differentiate layers", &layerDiffCheck))
                layerManager.setHighlightLayers(layerDiffCheck);

            if (ImGui::Checkbox("Show grid", &gridVisible))
                generateGrid(sf::Color::Red);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Custom hitbox"))
        {
            if (turboHitboxOverride)
            {
                layerManager.setActiveLayer(HITBOX_LAYER);

                sf::Vector2i press = pressedPos;
                sf::Vector2i release = releasedPos;
                swapStartAndStopPosition(press, release);
                static std::string input;
                sf::Vector2i start = (press - layerManager.getWorkAreaStart()) / TILE_SIZE;
                sf::Vector2i stop = (release - layerManager.getWorkAreaStart()) / TILE_SIZE;

                ImGui::Text("Selected area %d, %d -  %d, %d", start.x, start.y, stop.x, stop.y);
                handleLayerSelection(press, release);

                std::vector<CustomHitbox*> conflictingBoxes;
                for (CustomHitbox& box : hitbaxes)
                    if (sf::IntRect(start, stop - start + sf::Vector2i(1, 1)).intersects(sf::IntRect(box.min, box.max - box.min)))
                        conflictingBoxes.push_back(&box);

                if (!conflictingBoxes.empty())
                {
                    if (ImGui::BeginCombo("Conflicting boxes", conflictingBoxes[0]->flag.c_str()))
                    {
                        for (int i = 0; i < conflictingBoxes.size(); i++)
                        {
                            if (ImGui::Selectable(conflictingBoxes[i]->flag.c_str()))
                            {
                                pressedPos = conflictingBoxes[i]->min * TILE_SIZE + layerManager.getWorkAreaStart();
                                releasedPos = (conflictingBoxes[i]->max - sf::Vector2i(1, 1)) * TILE_SIZE + layerManager.getWorkAreaStart();
                                input = conflictingBoxes[i]->flag;
                            }

                            if (ImGui::IsItemHovered())
                            {
                                sf::Vector2i min = conflictingBoxes[i]->min * TILE_SIZE + layerManager.getWorkAreaStart();
                                sf::Vector2i max = (conflictingBoxes[i]->max - sf::Vector2i(1, 1)) * TILE_SIZE + layerManager.getWorkAreaStart();
                                handleLayerSelection(min, max);
                            }
                        }

                        ImGui::EndCombo();
                    }
                }

                ImGui::InputText("Collider flag", &input);



                if (ImGui::Button("Insert bax"))
                {
                    for (ActiveTile& tile : activeTiles)
                        tile.tileID = SPECIAL_HITBOX;

                    layerManager.insertTiles(activeTiles, press);

                    CustomHitbox hitbox;
                    hitbox.min = start;
                    hitbox.max = stop + sf::Vector2i(1, 1);
                    hitbox.flag = input;
                    hitbaxes.push_back(hitbox);
                }

                ImGui::SameLine();

                if (ImGui::Button("Delete conflicts"))
                {
                    for (int i = 0; i < hitbaxes.size(); i++)
                        if (sf::IntRect(start, stop - start + sf::Vector2i(1, 1)).intersects(sf::IntRect(hitbaxes[i].min, hitbaxes[i].max - hitbaxes[i].min)))
                            hitbaxes.erase(hitbaxes.begin() + i--);
                }
            }
            ImGui::Checkbox("Override!!!", &turboHitboxOverride);
            ImGui::Text("When overriding all blocks placed will have hitbox ID 999\n"
                "and left clicking is disabled. Right click to select \n"
                "an area to place the box in. All keyboard commands are also disabled");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Workspace"))
        {
            ImGui::Text("Current size: %d, %d", layerManager.getWorkAreaX(), layerManager.getWorkAreaY());

            ImGui::DragFloat2("New size", workArea, 1, 1, MAXIMUM_WORKSPACE);

            if (ImGui::Button("Resize"))
            {
                layerManager.resize(workArea[0], workArea[1]);
                generateGrid(sf::Color::Red);
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Lighting"))
        {
            ImGui::Checkbox("Render lights", &renderingLights);

            lightManager.updateMenu();
          

            ImGui::EndTabItem();
        }


        if (ImGui::BeginTabItem("Level info"))
        {
            infoWindow.update();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Help"))
        {
            ImGui::Text("Commands\nSave: CTRL + S\nUndo: CTRL + Z\nRedo: CTRL + Y\nCopy: CTRL + C\nCut: CTRL +X\nPaste: CTRL + V\nSelect eraser: E\nQuicksave: F5\nNumber buttons is used to change layers.");

            
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }


    ImGui::End();

    if (isImportingTexture)
    {
        ImGui::Begin("Texture importer", &isImportingTexture);

        std::vector<fs::path> paths = fileManager.getTexturePaths();
        for (auto& p : paths)
        {
            if (ImGui::Selectable(p.filename().string().c_str()))
            {
                importTexture(p.filename().string(), p.string());
            }
        }

        ImGui::End();
    }
}

void TileMenuHandler::handleBlockSelection(sf::Vector2i start, sf::Vector2i stop)
{
    bool cleared = false;

    start /= TILE_SIZE;
    stop /= TILE_SIZE;

    swapStartAndStopPosition(start, stop);

    int bound = 0;

    if (activeTileTexture != -1)
        bound = TileMaps::get().getSheetSize(activeTileTexture).x * TileMaps::get().getSheetSize(activeTileTexture).y;

    else if (layerManager.getActiveLayer() == LAYER_AMOUNT -1)
        bound = NR_OF_HITBOXES + 1;

    for (int k = start.y; k <= stop.y; k++)
    {
        for (int j = start.x; j <= stop.x; j++)
        {
            for (size_t i = 0; i < buttons.size(); i++)
            {
                if (buttons[i].isInside(float(j * TILE_SIZE + offset.x), float(k * TILE_SIZE + offset.y)))
                {
                    if (!cleared)
                    {
                        activeTiles.clear();

                        cleared = true;
                    }

                    if (i < bound)
                    {
                        ActiveTile activeTile;
                        activeTile.tileID = (int)i;
                        activeTile.textureID = activeTileTexture;
                        activeTile.x = j - start.x;
                        activeTile.y = k - start.y;
                        activeTile.box.setSize(TILE_SIZE, TILE_SIZE);
                        activeTile.box.setPosition(j * TILE_SIZE + offset.x, k * TILE_SIZE + offset.y);

                        if (layerManager.getActiveLayer() == LAYER_AMOUNT - 1)
                        {
                            activeTile.tileID = i + HITBOX_ID_START;
                            activeTile.textureID = 0;
                        }

                        activeTiles.push_back(activeTile);
                    }

                }
            }
        }
    }

    createActiveBounds(activeTiles);
}

void TileMenuHandler::handleLayerSelection(sf::Vector2i start, sf::Vector2i stop)
{
    swapStartAndStopPosition(start, stop);

    this->activeTiles = layerManager.getActiveTilesAt(start, stop);
    
    createActiveBounds(activeTiles);

}

void TileMenuHandler::createActiveBounds(std::vector<ActiveTile>& tiles) // TODO: Make this faster //
{
    //To figure out if this is a corner or nay
    for (ActiveTile & i : tiles)
    {
        bool leftNeighborExist = false;
        bool rightNeighborExist = false;
        bool topNeighborExist = false;
        bool bottomNeighborExist = false;

        for (const ActiveTile & neighbor : tiles)
        {
            if (abs(i.x - neighbor.x) + abs(i.y - neighbor.y) <= 1)
            {
                if (i.x < neighbor.x)
                    rightNeighborExist = true;

                else if (i.x > neighbor.x)
                    leftNeighborExist = true;

                else if (i.y < neighbor.y)
                    bottomNeighborExist = true;

                else if (i.y > neighbor.y)
                    topNeighborExist = true;
            }
        }

        if (leftNeighborExist && rightNeighborExist && topNeighborExist && bottomNeighborExist)
            i.box.setVisibleSide(Button::SIDES::NONE);



        else if (leftNeighborExist && rightNeighborExist && topNeighborExist)
            i.box.setVisibleSide(Button::SIDES::BOTTOM);

        else if (leftNeighborExist && rightNeighborExist && bottomNeighborExist)
            i.box.setVisibleSide(Button::SIDES::TOP);

        else if (leftNeighborExist && topNeighborExist && bottomNeighborExist)
            i.box.setVisibleSide(Button::SIDES::RIGHT);

        else if (rightNeighborExist && topNeighborExist && bottomNeighborExist)
            i.box.setVisibleSide(Button::SIDES::LEFT);



        else if (rightNeighborExist && leftNeighborExist)
            i.box.setVisibleSide(Button::SIDES::TOP_BOTTOM);

        else if (topNeighborExist && bottomNeighborExist)
            i.box.setVisibleSide(Button::SIDES::RIGHT_LEFT);



        else if (topNeighborExist && rightNeighborExist)
            i.box.setVisibleSide(Button::SIDES::BOTTOM_LEFT);

        else if (topNeighborExist && leftNeighborExist)
            i.box.setVisibleSide(Button::SIDES::BOTTOM_RIGHT);

        else if (bottomNeighborExist && rightNeighborExist)
            i.box.setVisibleSide(Button::SIDES::TOP_LEFT);

        else if (bottomNeighborExist && leftNeighborExist)
            i.box.setVisibleSide(Button::SIDES::TOP_RIGHT);


        else if (bottomNeighborExist)
            i.box.setVisibleSide(Button::SIDES::TOP_LEFT_RIGHT);

        else if (rightNeighborExist)
            i.box.setVisibleSide(Button::SIDES::TOP_LEFT_BOTTOM);

        else if (topNeighborExist)
            i.box.setVisibleSide(Button::SIDES::RIGHT_LEFT_BOTTOM);

        else if (leftNeighborExist)
            i.box.setVisibleSide(Button::SIDES::TOP_RIGHT_BOTTOM);

        else
            i.box.setVisibleSide(Button::SIDES::ENTIRE);
    }
}

void TileMenuHandler::swapStartAndStopPosition(sf::Vector2i & start, sf::Vector2i & stop)
{

    if (start.x > stop.x)
        std::swap(start.x, stop.x);

    if (start.y > stop.y)
        std::swap(start.y, stop.y);

}

void TileMenuHandler::setActiveTexture(sf::String name, sf::String path)
{
    activeTileTexture = TileMaps::get().getTextureIndex(name);
}

void TileMenuHandler::importTexture(std::string name, std::string path)
{
    activeTileTexture = fileManager.addTexture(name, path);
}

void TileMenuHandler::equipEraser()
{
    activeTiles.clear();
    ActiveTile eraser;
    eraser.tileID = -1;
    eraser.x = 0;
    eraser.y = 0;

    activeTiles.push_back(eraser);
    erasing = true;
}

bool TileMenuHandler::anyWindowsOpen()
{
    if (loadWindow.isOpen())
        return true;

    if (saveWindow.isOpen())
        return true;

    return false;
}

void TileMenuHandler::saveFile()
{
    std::string name = saveWindow.getFileName();
    fs::path dir = saveWindow.getPath();
    currentFileName = name;
    currentDir = dir;

    if (name.empty())
    {
        name = "defaultSaveName";
    }

    fileManager.save(layerManager, lightManager, hitbaxes, dir / name);
    infoWindow.saveInfo(fs::path(currentFileName).replace_extension(FILE_EXTENSION).string());
    saveWindow.closeWindow();
}

void TileMenuHandler::saveAsCurrentFile()
{
    if (currentFileName.size() == 0)
        saveWindow.openWindow();

    else
    {
        fileManager.save(layerManager, lightManager, hitbaxes, currentDir / currentFileName);
        infoWindow.saveInfo(fs::path(currentFileName).replace_extension(FILE_EXTENSION).string());
    }
}

void TileMenuHandler::loadFile()
{
    std::string name = loadWindow.getFilename();
    fs::path dir = loadWindow.getPath();
    currentFileName = name;
    currentDir = dir;

    fileManager.load(layerManager, lightManager, hitbaxes, dir / name);

    activeTiles.clear();
    activeTileTexture = TileMaps::get().getTexureCount() - 1;

    infoWindow.fetchInfo(name);
    loadWindow.closeWindow();
}

void TileMenuHandler::autosave()
{
    fileManager.save(layerManager, lightManager, hitbaxes, fs::current_path() / DEFAULT_SAVE_PATH / "autoSave");
}

void TileMenuHandler::load(std::string filename)
{
    fileManager.load(layerManager, lightManager, hitbaxes, fs::path(DEFAULT_SAVE_PATH) / filename);
    currentFileName = filename;
    currentDir = fs::path(DEFAULT_SAVE_PATH);
    activeTiles.clear();
    activeTileTexture = TileMaps::get().getTexureCount() - 1;

    infoWindow.fetchInfo(filename);
    loadWindow.closeWindow();
}

void TileMenuHandler::generateGrid(sf::Color color)
{
    grid.clear();
    grid.setPrimitiveType(sf::Lines);

    for (int i = 0; i < layerManager.getWorkAreaX(); i++)
    {
        sf::Vertex vertex(sf::Vector2f(layerManager.getWorkAreaStart().x + (i * TILE_SIZE), 0), color);
        grid.append(vertex);
        vertex.position.y = layerManager.getWorkAreaStart().y + (layerManager.getWorkAreaY() * TILE_SIZE);
        grid.append(vertex);
    }

    for (int i = 0; i < layerManager.getWorkAreaY(); i++)
    {
        sf::Vertex vertex(sf::Vector2f(0, layerManager.getWorkAreaStart().y + (i * TILE_SIZE)), color);
        grid.append(vertex);
        vertex.position.x = layerManager.getWorkAreaStart().x + (layerManager.getWorkAreaX() * TILE_SIZE);
        grid.append(vertex);
    }
}