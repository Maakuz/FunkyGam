#include "SaveWindow.h"
#include "Imgui/imgui.h"
#include "Imgui/misc/cpp/imgui_stdlib.h"
#include "LevelEditor/Constants.h"


SaveWindow::SaveWindow()
{
    open = false;
    readyToSave = false;
}

void SaveWindow::openWindow()
{
    open = true;

    currentDir = DEFAULT_SAVE_PATH;
}

void SaveWindow::closeWindow()
{
    open = false;
    readyToSave = false;
    fileName = "";
}

bool SaveWindow::update()
{
    if (open)
    {
        ImGui::Begin("Save", &open);
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
                            this->fileName = p.path().filename().string();
                    }
                }
                
            }

            if (ImGui::Selectable("../"))
            {
                iteratePaths("..", currentDir.parent_path().string());
            }

            ImGui::EndChild();
        }

        if (ImGui::InputText("Filename", &fileName, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)
            || ImGui::Button("Save"))
        {
            if (fileName != "")
                readyToSave = true;
        }

        static std::string folderName;
        if (ImGui::InputText("Folder name", &folderName, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)
            || ImGui::Button("Create folder"))
        {
            createFolder(folderName);
        }

        ImGui::End();
    }

    return readyToSave;
}

fs::path SaveWindow::getPath() const
{
    return currentDir;
}


void SaveWindow::iteratePaths(std::string name, std::string path)
{
    if (!(currentDir.root_path() == currentDir && name == ".."))
    {
        currentDir = std::string(path);
        if (currentDir.extension() != "")
            currentDir = currentDir.parent_path();
    }
}

void SaveWindow::createFolder(std::string folderName)
{
    fs::path newPath = currentDir;
    newPath /= folderName;

    try
    {
        fs::create_directory(newPath);
    }
    catch (const std::exception& e)
    {
        printf("%s", e.what());
    }
}
