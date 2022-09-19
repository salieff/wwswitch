#include "debounced_button.h"

WWSwitchDebouncedButton::WWSwitchDebouncedButton(uint8_t pin,
                                                 const WWSwitchDebouncedButton::TCallbackFunction &pressFunc,
                                                 const WWSwitchDebouncedButton::TCallbackFunction &holdFunc)
    : m_pinNumber(pin)
    , m_lastPinState(HIGH)
    , m_lastStableState(HIGH)
    , m_lastTimestamp(0)
    , m_pressFunction(pressFunc)
    , m_holdFunction(holdFunc)
    , m_holdFunctionFired(false)
{
}

void WWSwitchDebouncedButton::setup()
{
    pinMode(m_pinNumber, INPUT);
    m_lastTimestamp = millis();
    m_lastStableState = m_lastPinState = digitalRead(m_pinNumber);
}

void WWSwitchDebouncedButton::work()
{
    unsigned long timestamp = millis();
    int pinState = digitalRead(m_pinNumber);

    if (pinState != m_lastPinState)
    {
        m_lastPinState = pinState;
        m_lastTimestamp = timestamp;
        return;
    }

    if (timestamp - m_lastTimestamp < m_pressTime)
        return;

    if (m_lastStableState == HIGH && m_lastPinState == LOW)
    {
        m_pressFunction();
        m_holdFunctionFired = false;
    }

    m_lastStableState = m_lastPinState;

    if (m_holdFunctionFired)
        return;

    if (timestamp - m_lastTimestamp < m_holdTime)
        return;

    if (m_lastStableState == LOW)
    {
        m_holdFunction();
        m_holdFunctionFired = true;
    }
}
