#pragma once
#include <string>

class Item
{
public:
    Item();
    ~Item() {};

    int getID() const{ return id; };
    void setID(int id) { this->id = id; };

    std::string getName() const { return name; };
    void setName(std::string name) { this->name = name; };

    bool isStackable() const { return stackable; };
    void setStackable(bool stackable) { this->stackable = stackable; };

    int getStackSize() const { return stackSize; };
    void setStackSize(int size) { this->stackSize = size; };


private:
    int id;
    std::string name;
    bool stackable;
    int stackSize;
};

inline Item::Item()
{
    id = 0;
    stackable = false;
    stackSize = 0;
}