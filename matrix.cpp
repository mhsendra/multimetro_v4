#include "matrix.h"
#include "pcfwrapper.h"

uint8_t matrix_pcf_state = 0;

void matrix_disconnect_all()
{
    // Apagar SSR del ATmega
    digitalWrite(pin.SSR_LOW, LOW);
    digitalWrite(pin.SSR_MID, LOW);
    digitalWrite(pin.SSR_HIGH, LOW);
    digitalWrite(pin.SSR_SHUNT, LOW);

    // Apagar AQY por seguridad
    uint8_t s = pcf_read();
    s &= ~pcf.ZENER_AQY;
    pcf_write(s);

    matrix_pcf_state = s;
}

void matrix_restore()
{
    pcf_write(matrix_pcf_state);
}

// -------------------------------
// Test points (ADS1115 selecciona canal internamente)
// -------------------------------
void matrix_select_tp1()
{
    // El ADS1115 selecciona canal dentro de readADC()
}

void matrix_select_tp2()
{
    // El ADS1115 selecciona canal dentro de readADC()
}

void matrix_select_tp3()
{
    // El ADS1115 selecciona canal dentro de readADC()
}

// -------------------------------
// Shunts (corriente)
// -------------------------------
void matrix_shunt_low()
{
    digitalWrite(pin.SSR_SHUNT, HIGH);
}

void matrix_shunt_mid()
{
    digitalWrite(pin.SSR_SHUNT, HIGH);
}

void matrix_shunt_high()
{
    digitalWrite(pin.SSR_SHUNT, HIGH);
}

// -------------------------------
// Rangos OHM (74HC138)
// -------------------------------
void matrix_ohm_low()
{
    digitalWrite(pin.RNG0, LOW);
    digitalWrite(pin.RNG1, LOW);
    digitalWrite(pin.RNG2, LOW);
}

void matrix_ohm_mid()
{
    digitalWrite(pin.RNG0, HIGH);
    digitalWrite(pin.RNG1, LOW);
    digitalWrite(pin.RNG2, LOW);
}

void matrix_ohm_high()
{
    digitalWrite(pin.RNG0, LOW);
    digitalWrite(pin.RNG1, HIGH);
    digitalWrite(pin.RNG2, LOW);
}