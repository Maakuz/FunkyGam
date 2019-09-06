#pragma once
#include "SFML/Graphics/Shader.hpp"
#include <vector>


#define SHADER_FOLDER "../Shaders/"
#define SHADER_PATH(x) SHADER_FOLDER x
#define NR_OF_SHADERS 3

namespace SHADER
{
    enum shaders
    {
        lighting = 0,
        gaussVertical = 1,
        gaussHorizontal = 2
    };

}

class ShaderHandler 
{
public:
    ShaderHandler();
    ~ShaderHandler() {};

    sf::Shader& getShader(SHADER::shaders shader);

    sf::Shader& operator[](int i);


private:
    sf::Shader shaders[NR_OF_SHADERS];
};