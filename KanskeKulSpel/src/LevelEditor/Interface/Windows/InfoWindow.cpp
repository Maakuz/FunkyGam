#include "InfoWindow.h"
#include "Imgui/imgui.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"
#include "Game/Item/ItemHandler.h"
#include "Game/Handlers/CharacterHandler.h"
#include <fstream>

InfoWindow::InfoWindow()
{
    currentInfo = -1; 
}

void InfoWindow::update()
{
    if (ImGui::BeginChild("levelinfowindow"))
    {
        if (ImGui::BeginTabBar("infotabbber"))
        {
            for (LevelInfo& info : infos)
            {
                if (&info == &infos[currentInfo])
                    ImGui::PushStyleColor(ImGuiCol_Tab, sf::Color(200, 200, 100));

                std::string tablabel = "ID " + std::to_string(info.levelID);

                if (ImGui::BeginTabItem(tablabel.c_str()))
                {
                    ImGui::Text("File name: %s", info.levelFileName.c_str());
                    ImGui::InputText("Name", &info.name);
                    ImGui::InputTextMultiline("Description", &info.description);

                    ImGui::BeginChild("unlockableIDchild", sf::Vector2i(250, 200));

                    for (int i = 0; i < info.unlockLevels.size(); i++)
                    {
                        std::string label = "ULID " + std::to_string(i);
                        ImGui::InputInt(label.c_str(), &info.unlockLevels[i]);
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Unlockable level through exit%d", i);
                    }

                    if (ImGui::Button("Add exit"))
                        info.unlockLevels.push_back(0);

                    std::string buttonLabel = "Delete #" + std::to_string(info.unlockLevels.size() - 1);
                    if (!info.unlockLevels.empty() && ImGui::Button(buttonLabel.c_str()))
                        info.unlockLevels.pop_back();

                    ImGui::EndChild();

                    ImGui::SameLine();

                    ImGui::BeginChild("lootIDchild", sf::Vector2i(250, 200));

                    auto items = ItemHandler::getTemplateVec();
                    for (int i = 0; i < info.gatherables.size(); i++)
                    {
                        std::string label = "LootID #" + std::to_string(i);
                        if (ImGui::BeginCombo(label.c_str(), items->at(info.gatherables[i])->getName().c_str()))
                        {
                            for (int j = 0; j < items->size(); j++)
                            {
                                if (ImGui::Selectable(items->at(j)->getName().c_str()))
                                    info.gatherables[i] = j;
                            }
                            ImGui::EndCombo();
                        }
                    }

                    if (ImGui::Button("Add loot"))
                        info.gatherables.push_back(0);

                    buttonLabel = "Delete #" + std::to_string(info.gatherables.size() - 1);
                    if (!info.gatherables.empty() && ImGui::Button(buttonLabel.c_str()))
                        info.gatherables.pop_back();

                    ImGui::EndChild();

                    ImGui::BeginChild("rarelootIDchild", sf::Vector2i(250, 200));

                    for (int i = 0; i < info.rareGatherables.size(); i++)
                    {
                        std::string label = "LootID #" + std::to_string(i);
                        if (ImGui::BeginCombo(label.c_str(), items->at(info.rareGatherables[i])->getName().c_str()))
                        {
                            for (int j = 0; j < items->size(); j++)
                            {
                                if (ImGui::Selectable(items->at(j)->getName().c_str()))
                                    info.rareGatherables[i] = j;
                            }
                            ImGui::EndCombo();
                        }
                    }

                    if (ImGui::Button("Add Rare loot"))
                        info.rareGatherables.push_back(0);

                    buttonLabel = "Delete #" + std::to_string(info.rareGatherables.size() - 1);
                    if (!info.rareGatherables.empty() && ImGui::Button(buttonLabel.c_str()))
                        info.rareGatherables.pop_back();

                    ImGui::EndChild();

                    ImGui::SameLine();
                    ImGui::BeginChild("Enemieschilsd", sf::Vector2i(250, 200));

                    for (int i = 0; i < info.enemies.size(); i++)
                    {
                        std::string label = "Enemy #" + std::to_string(i);
                        if (ImGui::BeginCombo(label.c_str(), CharacterHandler::ENEMIES[info.enemies[i]].c_str()))
                        {
                            for (int j = 0; j < CharacterHandler::ENEMY_TEMPLATE_COUNT; j++)
                            {
                                if (ImGui::Selectable(CharacterHandler::ENEMIES[j].c_str()))
                                    info.enemies[i] = j;
                            }
                            ImGui::EndCombo();
                        }
                    }

                    if (ImGui::Button("Add Enemy"))
                        info.enemies.push_back(0);

                    buttonLabel = "Delete #" + std::to_string(info.enemies.size() - 1);
                    if (!info.enemies.empty() && ImGui::Button(buttonLabel.c_str()))
                        info.enemies.pop_back();

                    ImGui::EndChild();

                    ImGui::Checkbox("Has warp point", &info.hasWarpPoint);

                    ImGui::EndTabItem();
                }

                if (&info == &infos[currentInfo])
                    ImGui::PopStyleColor();

            }

            ImGui::EndTabBar();
        }
        ImGui::EndChild();
    }
}

void InfoWindow::fetchInfo(std::string fileName)
{
    infos.clear();
    std::ifstream file(DATA_PATH  "LevelInfo.mop");

    if (!file.is_open())
    {
        printf("Info not found aah\n");
        system("pause");
    }

    std::string trash;
    int count = 0;
    file >> trash;
    file >> trash >> count;

    for (int i = 0; i < count; i++)
    {
        LevelInfo info;
        file >> info;
        infos.push_back(info);
    }

    file.close();

    currentInfo = -1;
    for (int i = 0; i < infos.size(); i++)
    {
        if (infos[i].levelFileName == fileName)
            currentInfo = i;
    }

    if (currentInfo == -1)
    {
        LevelInfo info;
        info.name = fileName;
        info.description = "Info not found. Created new info entry for level";
        info.levelID = infos.size();
        info.levelFileName = "Does not exist";
        infos.push_back(info);
        currentInfo = infos.size() - 1;
    }
}

void InfoWindow::saveInfo(std::string fileName)
{
    infos[currentInfo].levelFileName = fileName;

    std::ofstream file(DATA_PATH "LevelInfo.mop");
    if (!file.is_open())
    {
        printf("Info not found aah\n");
        system("pause");
    }

    file << "[Levels]\n";
    file << "LevelCount: " << infos.size() << "\n\n";

    for (LevelInfo& info : infos)
        file << info << "\n";

    file.close();
}
