#include "FileManager.h"
#include <fstream>
#include "Constants.h"
#include "TileMaps.h"

namespace fs = std::filesystem;

FileManager::FileManager()
{
    this->quickSavePath = fs::current_path() / "Saves/";
}

void FileManager::save(const LayerManager & layerManager, const LightManager& lightManager, 
    const std::vector<CustomHitbox>& hitboxes, fs::path path) const
{
    path.replace_extension(FILE_EXTENSION);

    std::ofstream file(path);
    if (file.is_open())
    {
        file << layerManager << "\n";
        file << TileMaps::get() << "\n";
        file << lightManager << "\n";

        file << hitboxes.size() << " ";
        for (const CustomHitbox box : hitboxes)
            file << box;

        file.close();

        printf("Saved!\n");
    }

    else
        printf("NOT saved!\n");

}

void FileManager::quickSave(const LayerManager & layerManager, const LightManager& lightManager,
    const std::vector<CustomHitbox>& hitboxes) const
{
    
    fs::path fullPath = this->quickSavePath;
    fullPath.replace_filename(DEFAULT_QUICKSAVE_FILE_NAME);
    fullPath.replace_extension(FILE_EXTENSION);

    std::ofstream file(fullPath);
    if (file.is_open())
    {
        file << layerManager << "\n";
        file << TileMaps::get() << "\n";
        file << lightManager << "\n";

        file << hitboxes.size() << " ";
        for (const CustomHitbox box : hitboxes)
            file << box;

        file.close();

        printf("Quacksaved!\n");
    }

    else
        printf("NOT quacksaved!\n");
}

void FileManager::load(LayerManager & layerManager, LightManager& lightManager,
    std::vector<CustomHitbox>& hitboxes, fs::path path)
{
    std::ifstream file(path);
    if (file.is_open())
    {
        file >> layerManager;
        file >> TileMaps::get();
        file >> lightManager;

        int count;
        file >> count;
        hitboxes.clear();
        for (int i = 0; i < count; i++)
        {
            CustomHitbox box;
            file >> box;
            hitboxes.push_back(box);
        }
        file.close();

        printf("Loaded?!\n");
    }

    else 
        printf("NOT loaded!\n");
}

void FileManager::exportTextures(const LayerManager & layerManager)
{
    for (int i = 0; i < LAYER_AMOUNT; i++)
    {
        std::string fileName;
        fileName = "wat" + std::to_string(i) + ".png";
        layerManager.getLayerAsImage(i).saveToFile(fileName);
    }

    printf("exprot comprot\n");
}

std::vector<fs::path> FileManager::getTexturePaths()
{
    fs::path currentDir = fs::current_path();
    currentDir /= TILE_MAP_FOLDER;

    std::vector<fs::path> textures;

    for (auto & p : fs::directory_iterator(currentDir))
        textures.push_back(p);

    return textures;
}

int FileManager::addTexture(std::string name, std::string path)
{
    try
    {
        TileMaps::get().addTexture(name);
    }
    catch (const char* e)
    {
        printf("%s\n", e);
    }

    return TileMaps::get().getTextureIndex(name);
}
