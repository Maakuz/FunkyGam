#pragma once
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class SaveWindow
{
public:
    SaveWindow();
    virtual ~SaveWindow() {};

    bool isOpen() const { return open; };
    void openWindow();
    void closeWindow();
    bool update();
    fs::path getPath() const;
    std::string getFileName() const { return fileName; };
private:
    void iteratePaths(std::string name, std::string path);
    void createFolder(std::string folderName);

    bool open;
    bool readyToSave;
    fs::path currentDir;
    std::string fileName;
};