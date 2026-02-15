#ifndef IO_EXPANDER_PCF8574_H
#define IO_EXPANDER_PCF8574_H

#include "io_expander.h"
#include <PCF8574.h>

class IOExpanderPCF8574 : public IOExpander
{
public:
    IOExpanderPCF8574(PCF8574 *pcf, uint8_t i2cAddr); // <-- ahora recibe dirección
    void begin() override;
    void pinMode(uint8_t pin, uint8_t mode) override;
    void digitalWrite(uint8_t pin, uint8_t value) override;
    int digitalRead(uint8_t pin) override;

private:
    PCF8574 *_pcf;
    uint8_t _i2cAddr;
    bool _initialized = false;
};

#endif