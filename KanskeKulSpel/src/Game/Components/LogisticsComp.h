#pragma once
#include <string>
#include "Game/Components/Comp.h"

class LogisticsComp : public Comp
{
public:
    LogisticsComp();
    virtual ~LogisticsComp() {};

    static ComponentKey getStaticKey() { return ComponentKey::logistic; };
    Comp* clone() const { return new LogisticsComp(*this); };

    friend std::istream& operator>>(std::istream& in, LogisticsComp& item);
    friend std::ostream& operator<<(std::ostream& out, const LogisticsComp& item);

    void pluck() { this->obtained = true; }; //I just wanted to name it like that it could have been called obtainItem or something like that but nonono
    bool isObtained() const { return obtained; };

    int id;
    std::string name;
    int stackLimit;
    int emitterID;
    bool useable;

private:
    bool obtained;
};