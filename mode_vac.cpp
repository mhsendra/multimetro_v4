#include "mode_vac.h"
#include "adcmanager.h"
#include "globals.h"
#include "lcd_ui.h"
#include "config.h"
#include <math.h>
#include "auto_Hold.h"
#include "AutoOff.h"
#include "backlight.h"
#include "range_control.h"

// =====================================================
// CONFIGURACIÓN VAC
// =====================================================
#define VAC_SAMPLES 256

// =====================================================
// VAC — RAW (sin calibrar)
// =====================================================
float measureVAC_raw(void)
{
    const int N = VAC_SAMPLES;
    float sumSq = 0.0f;

    for (int i = 0; i < N; i++)
    {
        float v = adc_manager_read_voltage();

        if (fabs(v) > 4.95f)
            return INFINITY;

        sumSq += v * v;
    }

    float rms = sqrtf(sumSq / N);

    float scale = 1.0f;
    switch (adc_manager_current_range())
    {
    case RANGE_AC_2V:
        scale = 0.011f;
        break;
    case RANGE_AC_20V:
        scale = 0.110f;
        break;
    case RANGE_AC_200V:
        scale = 1.10f;
        break;
    default:
        return NAN;
    }

    return rms * scale;
}

// =====================================================
// VAC — CALIBRADO
// =====================================================
float measureVAC_calibrated(void)
{
    float v = measureVAC_raw();
    if (isinf(v))
        return v;
    return v * cal.vac;
}

// =====================================================
// VAC RELATIVO
// =====================================================
static float vac_reference = NAN;

float measureVAC_Relative(void)
{
    float v = measureVAC_calibrated();
    if (isnan(vac_reference))
        vac_reference = v;
    return v - vac_reference;
}

// =====================================================
// AUTO-RANGO VISUAL (mV / V) con histéresis
// =====================================================
static bool use_millivolts_vac(float v)
{
    static bool in_mV = false;
    if (v < 0.95f)
        in_mV = true;
    if (v > 1.05f)
        in_mV = false;
    return in_mV;
}

// =====================================================
// PANTALLAS
// =====================================================
void showVAC(void)
{
    float v = measureVAC_calibrated();
    if (autoHold_update(v))
        v = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);

    if (isinf(v))
    {
        lcd_driver_print(&lcd, "VAC: OVL");
        return;
    }

    lcd_driver_print(&lcd, "VAC: ");
    if (use_millivolts_vac(v))
    {
        lcd_driver_printFloat(&lcd, v * 1000.0f, 1);
        lcd_driver_print(&lcd, " mV");
    }
    else
    {
        lcd_driver_printFloat(&lcd, v, 3);
        lcd_driver_print(&lcd, " V");
    }
}

void showVAC_Relative(void)
{
    float v = measureVAC_Relative();
    lcd_ui_clear(&lcd);

    if (isinf(v))
    {
        lcd_driver_print(&lcd, "REL AC: OVL");
        return;
    }

    lcd_driver_print(&lcd, "REL AC: ");
    if (use_millivolts_vac(fabs(v)))
    {
        lcd_driver_printFloat(&lcd, v * 1000.0f, 1);
        lcd_driver_print(&lcd, " mV");
    }
    else
    {
        lcd_driver_printFloat(&lcd, v, 3);
        lcd_driver_print(&lcd, " V");
    }
}

// =====================================================
// MODO COMPLETO VAC
// =====================================================
void measureVAC_MODE(void)
{
    rng_release_for_gpio();

    backlight_activity();
    autoOff_activity();

    adc_manager_set_sps(ADC_SPS_250);
    adc_manager_select(RANGE_AC_20V);

    switch (vacSubMode)
    {
    case VAC_MAIN:
        showVAC();
        break;

    case VAC_REL:
        showVAC_Relative();
        break;
    }
}