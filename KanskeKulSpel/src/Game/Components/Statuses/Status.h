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
    Status(int duration, Statuses type);

    void setDuration(int duration) { m_duration = duration; };
    int getDuration() const { return m_duration; };
    Statuses getType() const { return m_type; };
    std::string getName();

    virtual void resolve(Entity* owner, float dt) = 0;

protected:
    int m_duration;
    std::string m_name;
    Statuses m_type;

private:
    friend class Entity;

    static const std::string s_statusNames[];

};