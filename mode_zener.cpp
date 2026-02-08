#include <Arduino.h>
#include "mode_zener.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "auto_hold.h"
#include "backlight.h"
#include "autoOff.h"
#include "pins.h"
#include "range_control.h"
#include "config.h"

/* =====================================================
 * MEDIR DIODO ZENER
 * ===================================================== */
float measureZener()
{
    rng_release_for_gpio(); // liberar RNG para este modo

    // Preparar pines
    pinMode(pin.TP1, OUTPUT);
    digitalWrite(pin.TP1, HIGH); // corriente de prueba
    pinMode(pin.TP2, INPUT);

    adc_manager_select(RANGE_DC_20V);
    adc_manager_set_sps(ADC_SPS_475);

    delay(10); // estabilizar

    float vz = adc_manager_read_voltage();

    // Saturación por protección
    if (vz > 4.95f)
        vz = 4.95f;

    // Reset de pines
    pinMode(pin.TP1, INPUT);
    pinMode(pin.TP2, INPUT);

    if (vz < 0.1f) // muy bajo, no detectado
        return NAN;

    return vz;
}

/* =====================================================
 * PANTALLA
 * ===================================================== */
void mode_zener_run()
{
    // Reset de sistemas auxiliares
    backlight_activity();
    autoHold_reset();
    autoOff_reset();

    lcd_ui_clear();
    lcd_ui_print("Detectando Zener...");
    delay(200);

    float vz = measureZener();

    if (!isnan(vz))
    {
        backlight_activity();
        autoOff_activity();
    }

    // --- AUTO HOLD ---
    if (autoHold_update(vz))
    {
        float held = autoHold_getHeldValue();

        lcd_ui_clear();
        lcd_ui_print("ZENER (HOLD)");
        lcd_ui_setCursor(0, 1);

        if (isnan(held))
        {
            lcd_ui_print("OL");
            return;
        }

        lcd_ui_print("Vz:");
        lcd_ui_printFloat(held, 2);
        lcd_ui_print(" V");
        return;
    }

    // --- Lectura normal ---
    lcd_ui_clear();
    lcd_ui_print("ZENER");
    lcd_ui_setCursor(0, 1);

    if (isnan(vz))
    {
        lcd_ui_print("OL");
        return;
    }

    lcd_ui_print("Vz:");
    lcd_ui_printFloat(vz, 2);
    lcd_ui_print(" V");
}