#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;



class LoadWindow
{
public:
    LoadWindow();
    virtual ~LoadWindow() {};

    bool isOpen() const { return open; };
    void openWindow();
    void closeWindow();
    bool update();
    fs::path getPath() const { return currentDir; };
    std::string getFilename() const { return fileName; };

private:
    void iteratePaths(std::string name, std::string path);

    bool open;
    bool readyToLoad;
    fs::path currentDir;
    std::string fileName;
};