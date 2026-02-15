#include "expander_selector.h"

ExpanderSelector::ExpanderSelector() : pinCount(0), mapCount(0) {}

void ExpanderSelector::addPin(IOExpander *expander, uint8_t pin)
{
    if (pinCount >= MAX_SELECTOR_PINS)
        return;
    pins[pinCount].expander = expander;
    pins[pinCount].pin = pin;
    pinCount++;
}

void ExpanderSelector::addMapping(uint8_t selValue, uint8_t mode, uint8_t currRange)
{
    if (mapCount >= MAX_SELECTOR_MAPS)
        return;
    mappings[mapCount].selValue = selValue;
    mappings[mapCount].mode = mode;
    mappings[mapCount].currRange = currRange;
    mapCount++;
}

void ExpanderSelector::updateMode(uint8_t &selectedMode, uint8_t &currentRange, uint8_t fallbackMode)
{
    uint8_t sel = 0;
    for (uint8_t i = 0; i < pinCount; i++)
    {
        if (!pins[i].expander)
            continue;
        int val = pins[i].expander->digitalRead(pins[i].pin);
        if (val != 0)
            sel |= (1 << i);
    }

    for (uint8_t i = 0; i < mapCount; i++)
    {
        if (mappings[i].selValue == sel)
        {
            selectedMode = mappings[i].mode;
            if (mappings[i].currRange != 0)
                currentRange = mappings[i].currRange;
            return;
        }
    }

    selectedMode = fallbackMode;
}