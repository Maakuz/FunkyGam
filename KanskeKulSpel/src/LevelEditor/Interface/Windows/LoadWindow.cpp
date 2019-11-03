#include "LoadWindow.h"
#include "Imgui/imgui.h"
#include "LevelEditor/Constants.h"

LoadWindow::LoadWindow()
{
    open = false;
    readyToLoad = false;
}

void LoadWindow::openWindow()
{
    open = true;

    currentDir = DEFAULT_SAVE_PATH;
}

void LoadWindow::closeWindow()
{
    open = false;
    readyToLoad = false;
}

bool LoadWindow::update()
{
    if (open)
    {
        ImGui::Begin("Open", &open);
        if (ImGui::BeginChild("Files", sf::Vector2f(200, 200)))
        {
            for (auto& p : fs::directory_iterator(currentDir))
            {
                bool file = false;
                bool directory = false;

                if (p.path().extension() == FILE_EXTENSION)
                    file = true;

                else if (p.path().extension() == "")
                    directory = true;

                if (file || directory)
                {
                    std::string name = p.path().filename().string();
                    if (directory)
                        name += "/";
                    if (ImGui::Selectable(name.c_str()))
                    {
                        if (directory)
                            iteratePaths(p.path().filename().string(), p.path().string());

                        if (file)
                        {
                            fileName = p.path().filename().string();
                            readyToLoad = true;
                        }
                    }
                }

            }

            if (ImGui::Selectable("../"))
            {
                iteratePaths("..", currentDir.parent_path().string());
            }

            ImGui::EndChild();
        }

        

        ImGui::End();
    }

    return readyToLoad;
}

void LoadWindow::iteratePaths(std::string name, std::string path)
{
    if (!(currentDir.root_path() == currentDir && name == ".."))
    {
        currentDir = std::string(path);
        if (currentDir.extension() != "")
            currentDir = currentDir.parent_path();
    }
}