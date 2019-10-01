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

    void queue(Light* light)
    {
        this->lights.push_back(light);
    }

    void clear()
    {
        lights.clear();
    }

    std::vector<Light*>& getQueue()
    {
        return lights;
    }

private:
    std::vector<Light*> lights;
};