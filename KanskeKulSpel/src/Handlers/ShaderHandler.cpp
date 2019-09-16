#include "ShaderHandler.h"


const std::string SHADER_NAMES[NR_OF_SHADERS] = {  "Lighting.frag" ,"shadowMapGenerator.frag", "GaussianVert.frag", "GaussianHor.frag" };
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


    
}

sf::Shader& ShaderHandler::getShader(SHADER::shaders shader)
{
    return shaders[shader];
}

sf::Shader& ShaderHandler::operator[](int i)
{
    return this->shaders[i];
}
