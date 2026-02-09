#ifndef PCF_WRAPPER_H
#define PCF_WRAPPER_H

#include <Arduino.h>
#include <PCF8574.h>
#include "globals.h" // ← Aquí está tu struct PCF original

#define PCF8574_ADDR 0x20

// El objeto pcf YA existe en pins.h, así que no lo declaramos aquí

uint8_t pcf_read();
void pcf_write(uint8_t value);

#endif