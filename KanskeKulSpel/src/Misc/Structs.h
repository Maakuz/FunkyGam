#pragma once

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
        in >> obj.flag;

        return in;
    }

};
