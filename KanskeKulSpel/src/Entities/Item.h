#pragma once

class Item
{
public:
    Item();
    ~Item() {};

    int getID() const{ return id; };
    void setID(int id) { this->id = id; };

private:
    int id;
};

inline Item::Item()
{
    id = 0;
}