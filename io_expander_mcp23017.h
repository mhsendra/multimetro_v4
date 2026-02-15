#ifndef IO_EXPANDER_MCP23017_H
#define IO_EXPANDER_MCP23017_H

#include <Adafruit_MCP23X17.h>
#include "io_expander.h"

class MCP23017Expander : public IOExpander
{
private:
    Adafruit_MCP23017 mcp;

public:
    void begin() override
    {
        mcp.begin();
    }

    void pinMode(uint8_t pin, uint8_t mode) override
    {
        mcp.pinMode(pin, mode);
    }

    void digitalWrite(uint8_t pin, uint8_t value) override
    {
        mcp.digitalWrite(pin, value);
    }

    int digitalRead(uint8_t pin) override
    {
        return mcp.digitalRead(pin);
    }

    // Métodos específicos del MCP23017
    uint8_t readPin(uint8_t pin)
    {
        return mcp.digitalRead(pin);
    }

    void writePin(uint8_t pin, uint8_t value)
    {
        mcp.digitalWrite(pin, value);
    }
};

#endif // IO_EXPANDER_MCP23017_H