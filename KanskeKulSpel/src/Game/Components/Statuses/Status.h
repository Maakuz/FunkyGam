#pragma once
#include "Game/Entities/Entity.h"
#include <string>

enum class Statuses
{
    regeneration
};

class Status
{
public:
    virtual void resolve(Entity* owner, float dt) = 0;

    void setDuration(int duration) { m_duration = duration; };

protected:
    int m_duration;
    std::string m_name;
    Statuses m_type;

private:
    friend class Entity;

};