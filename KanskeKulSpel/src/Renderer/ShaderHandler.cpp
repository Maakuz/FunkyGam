#include "ShaderHandler.h"
#include "Misc/ConsoleWindow.h"

#define SHADER_FOLDER "../Resources/Shaders/"
#define SHADER_PATH(x) SHADER_FOLDER x


const std::string SHADER_NAMES[NR_OF_SHADERS] = {  "Lighting.frag", "LightingNoShadow.frag" , "GaussianVert.frag", "GaussianHor.frag" };
sf::Shader ShaderHandler::shaders[NR_OF_SHADERS];

ShaderHandler::ShaderHandler()
{

    for (int i = 0; i < NR_OF_SHADERS; i++)
    {
        if (!this->shaders[i].loadFromFile(SHADER_PATH(+SHADER_NAMES[i]), sf::Shader::Type::Fragment))
        {
            printf("%s did not load correctly. Terminating everything!\n", SHADER_NAMES[i].c_str());
            system("pause");
            exit(-5);
        }
    }


    this->shaders[SHADER::lighting].setUniform("shadowMap", sf::Shader::CurrentTexture);
    this->shaders[SHADER::gaussHorizontal].setUniform("texture", sf::Shader::CurrentTexture);
    this->shaders[SHADER::gaussVertical].setUniform("texture", sf::Shader::CurrentTexture);

    ConsoleWindow::get().addCommand("reloadShaders", [&](Arguments args)->std::string 
        {
            if (reloadShaders())
                return "Reload sucessful";

            else
                return "Error, check termial.";
        });
    
}

sf::Shader& ShaderHandler::getShader(SHADER::shaders shader)
{
    return shaders[shader];
}

sf::Shader& ShaderHandler::operator[](int i)
{
    return this->shaders[i];
}

bool ShaderHandler::reloadShaders()
{
    bool success = true;

    for (int i = 0; i < NR_OF_SHADERS && success; i++)
    {
        sf::Shader temp;
        if (!temp.loadFromFile(SHADER_PATH(+SHADER_NAMES[i]), sf::Shader::Type::Fragment))
        {
            success = false;
        }

        else
        {
            if (!this->shaders[i].loadFromFile(SHADER_PATH(+SHADER_NAMES[i]), sf::Shader::Type::Fragment))
            {
                success = false;
            }
        }
    }


    return success;
}
