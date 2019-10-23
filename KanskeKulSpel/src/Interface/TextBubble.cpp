#include "TextBubble.h"

TextBubble::TextBubble(sf::Texture* texture)
{
    this->width = 0;
    this->height = 0;

    setTexture(texture);
}

TextBubble::TextBubble()
{
    this->width = 0;
    this->height = 0;
    this->texture = nullptr;
}

void TextBubble::setTexture(sf::Texture* texture)
{
    bubble.clear();
    bubble.setPrimitiveType(sf::Quads);
    this->size = texture->getSize();
    text.setPosition(this->size.x, this->size.y);

    sf::Vector2f poses[4] = { sf::Vector2f(0, 0), sf::Vector2f(size.x, 0), sf::Vector2f(size.x, size.y), sf::Vector2f(0, size.y) };
    sf::Vector2f minPoses[4] = { sf::Vector2f(size.x, 0), sf::Vector2f(size.x, size.y), sf::Vector2f(size.x, size.y), sf::Vector2f(0, size.y) };
    sf::Vector2f maxPoses[4] = { sf::Vector2f(0, size.y), sf::Vector2f(size.x, 0), sf::Vector2f(0, size.y), sf::Vector2f(size.x, 0) };

    for (int i = 0; i < 4; i++)
    {
        sf::Vertex vertices[4];
        vertices[0].position = sf::Vector2f(0, 0) + poses[i];
        vertices[1].position = sf::Vector2f(size.x, 0) + poses[i];
        vertices[2].position = sf::Vector2f(size.x, size.y) + poses[i];
        vertices[3].position = sf::Vector2f(0, size.y) + poses[i];
                           
        vertices[(i + 0) % 4].texCoords = sf::Vector2f(0, 0);
        vertices[(i + 1) % 4].texCoords = sf::Vector2f(size.x, 0);
        vertices[(i + 2) % 4].texCoords = sf::Vector2f(size.x, size.y);
        vertices[(i + 3) % 4].texCoords = sf::Vector2f(0, size.y);

        for (int j = 0; j < 4; j++)
        {
            bubble.append(vertices[j]);
        }       

        vertices[0].position = minPoses[i];
        vertices[1].position = minPoses[i] + sf::Vector2f(maxPoses[i].x, 0);
        vertices[2].position = minPoses[i] + maxPoses[i];
        vertices[3].position = minPoses[i] + sf::Vector2f(0, maxPoses[i].y);

        vertices[(i + 0) % 4].texCoords = sf::Vector2f(size.x, 0);
        vertices[(i + 1) % 4].texCoords = sf::Vector2f(size.x, 0);
        vertices[(i + 2) % 4].texCoords = sf::Vector2f(size.x, size.y);
        vertices[(i + 3) % 4].texCoords = sf::Vector2f(size.x, size.y);

        for (int j = 0; j < 4; j++)
        {
            bubble.append(vertices[j]);
        }
    }

    for (int j = 0; j < 4; j++)
    {
        bubble.append(sf::Vertex(sf::Vector2f(size.x, size.y), sf::Vector2f(size.x, size.y)));
    }
    
    this->texture = texture;
    text.setFillColor(sf::Color::Black);
}

void TextBubble::setWidth(unsigned int width) //pure witchcraft
{
    bubble[5].position.x += width - this->width;
    bubble[6].position.x += width - this->width;

    for (int i = 8; i < 20; i++)
        bubble[i].position.x += width - this->width;


    bubble[21].position.x += width - this->width;
    bubble[22].position.x += width - this->width;
    bubble[33].position.x += width - this->width;
    bubble[34].position.x += width - this->width;
}

void TextBubble::setHeight(unsigned int height)
{
    for (int i = 14; i < 28; i++)
    {
        bubble[i].position.y += height - this->height;
    }

    bubble[30].position.y += height - this->height;
    bubble[31].position.y += height - this->height;
    bubble[34].position.y += height - this->height;
    bubble[35].position.y += height - this->height;

}

void TextBubble::setPos(sf::Vector2f pos)
{
    this->pos = pos;
    sf::Transform t;
    t.translate(pos);

    transform = t;
}

void TextBubble::setFont(sf::Font* font)
{
    text.setFont(*font);
}

void TextBubble::setCharacterSize(unsigned int size)
{
    this->text.setCharacterSize(size);
}

void TextBubble::setText(std::string text)
{
    this->text.setString(text);
}

void TextBubble::appendText(std::string text)
{
    this->text.setString(this->text.getString() + text);
}

void TextBubble::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = texture;
    states.transform = transform;
    target.draw(bubble, states);
    states.texture = nullptr;
    target.draw(text, states);
}
