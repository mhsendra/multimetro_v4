#include "io_expander_pcf8574.h"
#include <Arduino.h> // para Serial

IOExpanderPCF8574::IOExpanderPCF8574(PCF8574 *pcf, uint8_t i2cAddr)
    : _pcf(pcf), _i2cAddr(i2cAddr), _initialized(false)
{
}

void IOExpanderPCF8574::begin()
{
    _initialized = _pcf->begin(_i2cAddr); // retorna true si inicializó correctamente

    if (!_initialized)
    {
        Serial.print("Error: PCF8574 no inicializado en dirección 0x");
        Serial.println(_i2cAddr, HEX);
        Serial.println("Todas las operaciones con este expander serán ignoradas.");
    }
}

void IOExpanderPCF8574::pinMode(uint8_t pin, uint8_t mode)
{
    if (!_initialized)
        return;

    // PCF8574 es quasi-bidireccional: escribir HIGH simula INPUT_PULLUP
    if (mode == OUTPUT)
    {
        _pcf->write(pin, LOW); // salida por defecto
    }
    else if (mode == INPUT_PULLUP)
    {
        _pcf->write(pin, HIGH); // pull-up interno
    }
}

void IOExpanderPCF8574::digitalWrite(uint8_t pin, uint8_t value)
{
    if (!_initialized)
        return;

    _pcf->write(pin, value);
}

int IOExpanderPCF8574::digitalRead(uint8_t pin)
{
    if (!_initialized)
        return LOW;

    return _pcf->read(pin);
}