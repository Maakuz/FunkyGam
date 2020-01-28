#include "StatusComp.h"
#include <fstream>
#include <algorithm>
#include "Game/Components/Statuses/Regeneration.h"

std::istream& operator>>(std::istream& in, StatusComp& status)
{
    std::string trash;
    int count;
    in >> trash;
    in >> trash >> count;

    status.clear();

    for (int i = 0; i < count; i++)
    {
        int id = 0;
        int duration = 0;

        in >> id >> duration;

        status.addStatusEffect((Statuses)id, duration);
    }


    return in;
}

std::ostream& operator<<(std::ostream& out, const StatusComp& status)
{
    out << "[StatusComp]\n";
    out << "Count: " << status.m_statuses.size() << "\n";

    for (auto& pair : status.m_statuses)
        out << (int)pair.first << " " << pair.second->getDuration() << "\n";

    return out;
}

StatusComp::~StatusComp()
{
    clear();
}

void StatusComp::clear()
{
    for (auto& pair : m_statuses)
        delete pair.second;

    m_statuses.clear();
}


void StatusComp::resolveStatusEffects(Entity* owner, float dt)
{
}

void StatusComp::addStatusEffect(Statuses status, int duration)
{
    if (m_statuses.count(status))
        m_statuses[status]->setDuration(std::max(m_statuses[status]->getDuration(), duration));

    else
    {
        switch (status)
        {
        case Statuses::regeneration:
            m_statuses.emplace(status, new Regeneration(duration));
            break;
        default:
            break;
        }
    }
}

void StatusComp::removeStatusEffect(Statuses status)
{
    if (m_statuses.count(status))
    {
        delete m_statuses[status];
        m_statuses.erase(status);
    }
}

void StatusComp::addFromStatusComp(const StatusComp& other)
{
    for (auto& pair : *other.getStatusPtr())
        addStatusEffect(pair.first, pair.second->getDuration());
}

StatusComp::StatusList StatusComp::getTemplateList()
{
     StatusList list;
     list.emplace(Statuses::regeneration, nullptr);

    return list;
}