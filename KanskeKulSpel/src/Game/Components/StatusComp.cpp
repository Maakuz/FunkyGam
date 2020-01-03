#include "StatusComp.h"
#include <fstream>

const std::string StatusComp::STATUS_NAME[] = { "Regeneration" };

std::istream& operator>>(std::istream& in, StatusComp& status)
{
    return in;
}

std::ostream& operator<<(std::ostream& out, const StatusComp& status)
{
    out << "Count: " << status.m_statuses.size() << "\n";

    for (auto& pair : status.m_statuses)
        out << (int)pair.first << " " << pair.second << "\n";

    return out;
}

 StatusComp::StatusList StatusComp::getTemplateList()
{
     StatusList list;
     list.emplace(Status::regeneration, 0);

    return list;
}
