#include "Status.h"

const std::string Status::s_statusNames[] = {"Regeneration"};

Status::Status(int duration, Statuses type)
{
    m_duration = duration;
    m_type = type;
}

std::string Status::getName()
{
    return s_statusNames[(int)m_type];
}
