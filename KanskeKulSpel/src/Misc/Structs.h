#pragma once
#include <istream>
#include <ostream>

struct Tile
{
    Tile()
    {
        color = sf::Color::White;
        x = 0;
        y = 0;
        textureID = -1;
        tileID = -1;
    };
    int x;
    int y;
    //What texture.
    int textureID;
    //Where in the texture.
    int tileID;

    //an overlay color
    sf::Color color;
};



struct CustomHitbox
{
    sf::Vector2i min;
    sf::Vector2i max;
    std::string flag;

    friend std::ostream& operator<<(std::ostream& out, const CustomHitbox& obj)
    {
        out << obj.min.x << " " << obj.min.y << " ";
        out << obj.max.x << " " << obj.max.y << " ";
        out << obj.flag << "\n";

        return out;
    }

    friend std::istream& operator>>(std::istream& in, CustomHitbox& obj)
    {
        in >> obj.min.x >> obj.min.y;
        in >> obj.max.x >> obj.max.y;
        std::getline(in, obj.flag);
        while (obj.flag.size() > 0 && obj.flag[0] == ' ')
            obj.flag.erase(obj.flag.begin());

        return in;
    }

};
