#ifndef IO_EXPANDER_UTILS_H
#define IO_EXPANDER_UTILS_H

#include "io_expander.h"
#include <Arduino.h>

namespace IOExpanderUtils
{

    // Escribe un valor de 8 bits en un IOExpander
    void write8(IOExpander &expander, uint8_t value);

    // Escribe un valor de 16 bits en un IOExpander (si soporta 16 pines)
    void write16(IOExpander &expander, uint16_t value);

    // Lee un valor de 8 bits desde un IOExpander
    uint8_t read8(IOExpander &expander);

    // Lee un valor de 16 bits desde un IOExpander
    uint16_t read16(IOExpander &expander);

} // namespace IOExpanderUtils

#endif