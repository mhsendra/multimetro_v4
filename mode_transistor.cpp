#include "mode_transistor.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "globals.h"
#include "config.h"
#include "range_control.h"
#include "backlight.h"
#include "auto_hold.h"
#include "autoOff.h"
#include <Arduino.h>

// Pines fijos según el nuevo hardware
static const uint8_t PIN_E = pin.TP1; // NET_COM
static const uint8_t PIN_C = pin.TP2; // NET_VOHM_IN
static const uint8_t PIN_B = pin.TP3; // POWER_HOLD → R55 → Base

/* =====================================================
 * MEDIR hFE (solo NPN)
 * ===================================================== */
float measureHFE()
{
    // Polarizar base (NPN → HIGH)
    pinMode(PIN_B, OUTPUT);
    digitalWrite(PIN_B, HIGH);
    delayMicroseconds(200);

    // Seleccionar rango de transistor
    adc_manager_select(RANGE_TRANSISTOR);
    adc_manager_set_sps(ADC_SPS_860);

    float vc = adc_manager_read_voltage();

    // Protección por saturación del ADC
    if (vc > 4.95f)
        return NAN;

    // Liberar base
    pinMode(PIN_B, INPUT);

    // Cálculo simple de hFE
    float hfe = (OHM_VREF - vc) * 100.0f;
    return hfe;
}

/* =====================================================
 * PANTALLA
 * ===================================================== */
void showTransistor()
{
    backlight_activity();
    autoHold_reset();
    autoOff_reset();

    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "Transistor NPN");
    delay(300);

    float hfe = measureHFE();

    backlight_activity();
    autoOff_activity();

    // --- AUTO HOLD ---
    if (autoHold_update(hfe))
    {
        float held = autoHold_getHeldValue();

        lcd_ui_clear(&lcd);
        lcd_driver_print(&lcd, "NPN (HOLD)");
        lcd_ui_setCursor(&lcd, 0, 1);
        lcd_driver_print(&lcd, "hFE:");
        lcd_driver_printFloat(&lcd, held, 0);
        return;
    }

    // --- Lectura normal ---
    lcd_ui_clear(&lcd);
    lcd_driver_print(&lcd, "NPN");
    lcd_ui_setCursor(&lcd, 0, 1);
    lcd_driver_print(&lcd, "hFE:");
    lcd_driver_printFloat(&lcd, hfe, 0);
}