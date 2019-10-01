#pragma once


class Counter
{
public:
    int counter;
    int stopValue;

    Counter(int stopValue = 0)
    {
        this->counter = 0;
        this->stopValue = stopValue;
    }

    ~Counter() {};

    bool update(int increment)
    {
        counter += increment;
        return counter > stopValue;
    }

    void reset()
    {
        counter = 0;
    }

    bool isTimeUp()
    {
        return counter > stopValue;
    }
};