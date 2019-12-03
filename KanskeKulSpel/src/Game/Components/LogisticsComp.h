#pragma once
#include <string>

class LogisticsComp
{
public:
    LogisticsComp();
    virtual ~LogisticsComp() {};

private:
    int id;
    std::string name;
    int stackLimit;
    int emitterID;
    bool useable;
    bool obtained;
};