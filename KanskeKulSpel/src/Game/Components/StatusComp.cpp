#include "StatusComp.h"
#include <fstream>
#include <algorithm>

const std::string StatusComp::STATUS_NAME[] = { "Regeneration" };

std::istream& operator>>(std::istream& in, StatusComp& status)
{
    std::string trash;
    int count;
    in >> trash;
    in >> trash >> count;

    for (int i = 0; i < count; i++)
    {
        int id = 0;
        int duration = 0;

        in >> id >> duration;

        status.m_statuses.emplace((StatusComp::Status)id, duration);
    }


    return in;
}

std::ostream& operator<<(std::ostream& out, const StatusComp& status)
{
    out << "[StatusComp]\n";
    out << "Count: " << status.m_statuses.size() << "\n";

    for (auto& pair : status.m_statuses)
        out << (int)pair.first << " " << pair.second << "\n";

    return out;
}

void StatusComp::addFromStatusComp(const StatusComp& other)
{
    for (auto& pair : *other.getStatusPtr())
    {
        if (m_statuses.count(pair.first))
            m_statuses[pair.first] = std::max(m_statuses[pair.first], pair.second);
        
        else
            m_statuses.emplace(pair.first, pair.second);
    }
}

StatusComp::StatusList StatusComp::getTemplateList()
{
     StatusList list;
     list.emplace(Status::regeneration, 0);

    return list;
}
