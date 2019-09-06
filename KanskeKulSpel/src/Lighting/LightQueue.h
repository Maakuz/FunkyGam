#pragma once
#include "Light.h"
#include <vector>

class LightQueue
{
public:
    static LightQueue & get()
    {
        static LightQueue queue;
        return queue;
    }
    virtual ~LightQueue() {};

    void queue(Light light)
    {
        this->light.push_back(light);
    }

    void clear()
    {
        light.clear();
    }

    std::vector<Light>& getQueue()
    {
        return light;
    }

private:
    std::vector<Light> light;
};