#ifndef WWSWITCH_DEBOUNCED_BUTTON_H
#define WWSWITCH_DEBOUNCED_BUTTON_H

#include <Arduino.h>

#define LEFT_BUTTON_PIN_NUMBER 5
#define RIGHT_BUTTON_PIN_NUMBER 12


class WWSwitchDebouncedButton {
public:
    using TCallbackFunction = std::function<void(void)>;

    WWSwitchDebouncedButton(uint8_t pin, const TCallbackFunction &pressFunc, const TCallbackFunction &holdFunc);

    void setup();
    void work();

private:
    uint8_t m_pinNumber;
    int m_lastPinState;
    int m_lastStableState;
    unsigned long m_lastTimestamp;

    TCallbackFunction m_pressFunction;
    TCallbackFunction m_holdFunction;
    bool m_holdFunctionFired;

    static const unsigned long m_pressTime = 50;
    static const unsigned long m_holdTime = 10000;
};

#endif // WWSWITCH_DEBOUNCED_BUTTON_H
