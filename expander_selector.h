#ifndef EXPANDER_SELECTOR_H
#define EXPANDER_SELECTOR_H

#include "io_expander.h"
#include <Arduino.h>
#include <stdint.h>

#define MAX_SELECTOR_PINS 8
#define MAX_SELECTOR_MAPS 8

struct SelectorPin
{
    IOExpander *expander;
    uint8_t pin;
};

struct SelectorMapping
{
    uint8_t selValue;
    uint8_t mode;
    uint8_t currRange;
};

class ExpanderSelector
{
public:
    ExpanderSelector();

    void addPin(IOExpander *expander, uint8_t pin);
    void addMapping(uint8_t selValue, uint8_t mode, uint8_t currRange = 0);

    void updateMode(uint8_t &selectedMode, uint8_t &currentRange, uint8_t fallbackMode);

private:
    SelectorPin pins[MAX_SELECTOR_PINS];
    uint8_t pinCount;

    SelectorMapping mappings[MAX_SELECTOR_MAPS];
    uint8_t mapCount;
};

#endif