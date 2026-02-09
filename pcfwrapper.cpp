#include "pcfwrapper.h"
#include "globals.h"

// Objeto real de la librería de Tillaart
PCF8574 expander(PCF8574_ADDR);

uint8_t pcf_read()
{
    return expander.read8();
}

void pcf_write(uint8_t value)
{
    expander.write8(value);
}