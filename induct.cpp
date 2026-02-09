#include <Arduino.h>
#include <math.h>
#include "induct.h"
#include "mode_current.h"
#include "globals.h"
#include "lcd_ui.h"
#include "config.h"
#include "filters.h"
#include "adcmanager.h"
#include "auto_hold.h"
#include "backlight.h"
#include "autoOff.h"
#include "range_control.h"

/* =====================================================
 * MEDICIÓN RAW DE INDUCTANCIA (Henrios)
 * ===================================================== */
float measureInductance_raw()
{
    rng_release_for_gpio(); // liberar RNG para este modo

    // 1) Descarga previa
    pinMode(pin.TP1, OUTPUT);
    pinMode(pin.TP2, OUTPUT);
    digitalWrite(pin.TP1, LOW);
    digitalWrite(pin.TP2, LOW);
    delay(5);

    // 2) Carga
    pinMode(pin.TP1, OUTPUT);
    pinMode(pin.TP2, INPUT);
    digitalWrite(pin.TP1, HIGH);
    delay(200);

    float i0 = measureCURRENT_calibrated() * 1000.0f; // A → mA

    // 3) Descarga parcial
    digitalWrite(pin.TP1, LOW);
    delayMicroseconds(150);

    float i1 = measureCURRENT_calibrated() * 1000.0f;

    // Restaurar pines
    pinMode(pin.TP1, INPUT);
    pinMode(pin.TP2, INPUT);

    // 4) Cálculo
    const float R_internal = 47.0f;
    const float t = 0.000150f;

    if (i1 <= 0 || i0 <= i1)
        return NAN;

    float ratio = i0 / i1;
    float lnval = log(ratio);
    if (lnval < 0.000001f)
        return NAN;

    return (t * R_internal) / lnval;
}

/* =====================================================
 * MEDICIÓN CALIBRADA
 * ===================================================== */
float measureInductance_calibrated()
{
    float L = measureInductance_raw();
    if (isnan(L))
        return NAN;

    return L * cal.induct_factor;
}

/* =====================================================
 * PANTALLA
 * ===================================================== */
void showInductance()
{
    // Reset de sistemas auxiliares
    backlight_activity();
    autoHold_reset();
    autoOff_reset();

    lcd_ui_clear();
    lcd_ui_print("Detectando...");
    delay(200);

    float L = measureInductance_calibrated();

    // Registrar actividad
    if (!isnan(L) && L > 0)
    {
        backlight_activity();
        autoOff_activity();
    }

    // --- AUTO HOLD ---
    if (autoHold_update(L))
    {
        float held = autoHold_getHeldValue();

        lcd_ui_clear();
        lcd_ui_print("L (HOLD)");
        lcd_ui_setCursor(0, 1);

        if (isnan(held) || held <= 0)
        {
            lcd_ui_print("OUT OF RANGE");
            return;
        }

        if (held < 1e-9)
        {
            lcd_ui_print("OUT OF RANGE");
        }
        else if (held < 1e-6)
        {
            lcd_ui_printFloat(held * 1e9, 1);
            lcd_ui_print(" nH");
        }
        else if (held < 1e-3)
        {
            lcd_ui_printFloat(held * 1e6, 1);
            lcd_ui_print(" uH");
        }
        else
        {
            lcd_ui_printFloat(held * 1e3, 1);
            lcd_ui_print(" mH");
        }
        return;
    }

    // --- Lectura normal ---
    lcd_ui_clear();
    lcd_ui_print("L:");
    lcd_ui_setCursor(0, 1);

    if (isnan(L) || L <= 0)
    {
        lcd_ui_print("OUT OF RANGE");
        return;
    }

    if (L < 1e-9)
    {
        lcd_ui_print("OUT OF RANGE");
    }
    else if (L < 1e-6)
    {
        lcd_ui_printFloat(L * 1e9, 1);
        lcd_ui_print(" nH");
    }
    else if (L < 1e-3)
    {
        lcd_ui_printFloat(L * 1e6, 1);
        lcd_ui_print(" uH");
    }
    else
    {
        lcd_ui_printFloat(L * 1e3, 1);
        lcd_ui_print(" mH");
    }
}

/* =====================================================
 * API PÚBLICA DEL MODO INDUCTANCIA
 * ===================================================== */
void measureInductanceMode()
{
    showInductance();
}