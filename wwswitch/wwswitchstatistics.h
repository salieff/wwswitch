#ifndef WWSWITCHSTATISTICS_H
#define WWSWITCHSTATISTICS_H

#include <Arduino.h>


class WWSwitchStatistics
{
public:
    void work();
    String getStatistics();

private:
    unsigned long m_lastMillis = 0;

    unsigned long m_upTimeDays = 0;
    unsigned long m_upTimeHours = 0;
    unsigned long m_upTimeMinutes = 0;
    unsigned long m_upTimeSeconds = 0;
    unsigned long m_upTimeMillis = 0;
};

#endif // WWSWITCHSTATISTICS_H
