#include "wwswitchstatistics.h"

void WWSwitchStatistics::work()
{
    unsigned long delta = millis() - m_lastMillis;
    m_lastMillis += delta;

    m_upTimeMillis += delta;

    m_upTimeSeconds += m_upTimeMillis / 1000ul;
    m_upTimeMillis %= 1000ul;

    m_upTimeMinutes += m_upTimeSeconds / 60ul;
    m_upTimeSeconds %= 60ul;

    m_upTimeHours += m_upTimeMinutes / 60ul;
    m_upTimeMinutes %= 60ul;

    m_upTimeDays += m_upTimeHours / 24ul;
    m_upTimeHours %= 24ul;
}

String WWSwitchStatistics::getStatistics()
{
    String retStr;

#define ADD_ESP2(arg, arg2) retStr += #arg": "; retStr += String(ESP.get##arg()) + arg2 + "\n";
#define ADD_ESP(arg) ADD_ESP2(arg, "")

    ADD_ESP(Vcc);
    ADD_ESP(ChipId);
    ADD_ESP(FreeHeap);
    ADD_ESP(MaxFreeBlockSize);
    ADD_ESP2(HeapFragmentation, "%");
    ADD_ESP(FreeContStack);
    ADD_ESP(SdkVersion);
    ADD_ESP(CoreVersion);
    ADD_ESP(FullVersion);
    ADD_ESP(BootVersion);
    ADD_ESP(BootMode);
    ADD_ESP(CpuFreqMHz);
    ADD_ESP(FlashChipId);
    ADD_ESP(FlashChipVendorId);
    ADD_ESP(FlashChipRealSize);
    ADD_ESP(FlashChipSize);
    ADD_ESP(FlashChipSpeed);
    ADD_ESP(FlashChipMode);
    ADD_ESP(FlashChipSizeByChipId);
    ADD_ESP(SketchSize);
    ADD_ESP(SketchMD5);
    ADD_ESP(FreeSketchSpace);
    ADD_ESP(ResetReason);
    ADD_ESP(ResetInfo);
    ADD_ESP(CycleCount);

#undef ADD_ESP2
#undef ADD_ESP

    retStr += "Uptime: " + String(m_upTimeDays) + "d " +
            String(m_upTimeHours) + ":" +
            String(m_upTimeMinutes) + ":" +
            String(m_upTimeSeconds) + "." +
            String(m_upTimeMillis) + "\n";

    return retStr;
}
