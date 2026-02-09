#include "mode_current.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "globals.h"
#include "auto_Hold.h"
#include "backlight.h"
#include "AutoOff.h"
#include "range_control.h"
#include <math.h>

// =====================================================
// CORRIENTE mA y 5A (shunt + LM358 → ADS1115 canal shunt)
// =====================================================
static float measureCurrent_Shunt_RAW(void)
{
    // Seleccionar rango ADC según rango lógico
    if (currentRange == CURR_RANGE_mA)
        adc_manager_select(RANGE_CURR_20mA);
    else // CURR_RANGE_5A
        adc_manager_select(RANGE_CURR_200mA);

    // Leer voltaje directamente del ADS1115
    float v = adc_manager_read_voltage();

    // Protección por saturación
    if (fabs(v) > 4.95f)
        return INFINITY;

    // Aplicar offset del amplificador
    v -= cal.curr_shunt_offset;

    // Convertir a corriente (A)
    return v * cal.curr_shunt_gain;
}

// =====================================================
// CORRIENTE 16A (ACS712 → ADS1115 canal ACS)
// =====================================================
static float measureCurrent_ACS_RAW(void)
{
    adc_manager_select(RANGE_CURR_16A);

    float v = adc_manager_read_voltage();

    // Protección por saturación
    if (fabs(v) > 4.95f)
        return INFINITY;

    // Aplicar offset calibrado
    v -= cal.acs_offset;

    // Convertir a corriente (A)
    return v / cal.acs_sens;
}

// =====================================================
// DISPATCHER RAW
// =====================================================
float measureCURRENT_RAW(void)
{
    // Liberar pines RNGx si se estaba usando modo OHM
    rng_release_for_gpio();

    switch (currentRange)
    {
    case CURR_RANGE_mA:
    case CURR_RANGE_5A:
        return measureCurrent_Shunt_RAW();
    case CURR_RANGE_16A:
        return measureCurrent_ACS_RAW();
    default:
        return NAN;
    }
}

// =====================================================
// CALIBRADO (wrapper limpio)
// =====================================================
float measureCURRENT_calibrated(void)
{
    // Las funciones RAW ya aplican calibración (offset/gain/sens)
    return measureCURRENT_RAW();
}

// =====================================================
// PANTALLA (con filtro EMA, backlight, autoOff, autoHold)
// =====================================================
void showCURRENT(void)
{
    static float If = -1.0f;

    float i = measureCURRENT_calibrated();

    // Actividad si la lectura es válida
    if (!isinf(i) && !isnan(i))
    {
        backlight_activity();
        autoOff_activity();
    }

    if (autoHold_update(i))
        i = autoHold_getHeldValue();

    lcd_ui_clear(&lcd);

    if (isinf(i))
    {
        lcd_driver_print(&lcd, "I: OVL");
        return;
    }

    // Filtro EMA para suavizar ruido
    If = applyEMA(i, If, 0.20f);

    if (currentRange == CURR_RANGE_mA)
    {
        lcd_driver_print(&lcd, "I: ");
        lcd_driver_printFloat(&lcd, If * 1000.0f, 1);
        lcd_driver_print(&lcd, " mA");
    }
    else
    {
        lcd_driver_print(&lcd, "I: ");
        lcd_driver_printFloat(&lcd, If, 3);
        lcd_driver_print(&lcd, " A");
    }
}

// =====================================================
// MODO COMPLETO
// =====================================================
void measureCURRENT(void)
{
    // Velocidad del ADS1115 adecuada para corriente
    adc_manager_set_sps(ADC_SPS_475);

    showCURRENT();
}