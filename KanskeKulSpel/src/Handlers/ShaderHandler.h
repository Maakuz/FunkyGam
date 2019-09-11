#pragma once
#include "SFML/Graphics/Shader.hpp"
#include <vector>


#define SHADER_FOLDER "../Shaders/"
#define SHADER_PATH(x) SHADER_FOLDER x
#define NR_OF_SHADERS 4

namespace SHADER
{
    enum shaders
    {
        lighting = 0,
        shadowMap = 1,
        gaussVertical = 2,
        gaussHorizontal = 3
    };

}

class ShaderHandler 
{
public:
    ShaderHandler();
    ~ShaderHandler() {};

    sf::Shader& getShader(SHADER::shaders shader);

    sf::Shader& operator[](int i);

    const int lightingPass[3] = {SHADER::shaders::lighting, SHADER::shaders::gaussVertical, SHADER::shaders::gaussHorizontal};

private:
    sf::Shader shaders[NR_OF_SHADERS];
};