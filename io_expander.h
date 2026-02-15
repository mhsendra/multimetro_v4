#ifndef IO_EXPANDER_H
#define IO_EXPANDER_H

#include <Arduino.h>

class IOExpander
{
public:
    virtual void begin() = 0;
    virtual void pinMode(uint8_t pin, uint8_t mode) = 0;
    virtual void digitalWrite(uint8_t pin, uint8_t value) = 0;
    virtual int digitalRead(uint8_t pin) = 0;
    virtual ~IOExpander() {}
};

class MCP23017Expander : public IOExpander
{
public:
    void begin() override;
    void pinMode(uint8_t pin, uint8_t mode) override;
    void digitalWrite(uint8_t pin, uint8_t value) override;
    int digitalRead(uint8_t pin) override;

    uint8_t readPin(uint8_t pin);
    void writePin(uint8_t pin, uint8_t value);
};

#endif // IO_EXPANDER_H