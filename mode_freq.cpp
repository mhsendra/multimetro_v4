#include "mode_freq.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "globals.h"
#include "pins.h"
#include "config.h"
#include "auto_Hold.h"
#include "backlight.h"
#include "AutoOff.h"
#include <math.h>
#include "range_control.h"

// =====================================================
// LECTURA ANALÓGICA PARA FRECUENCIA (MISMO NODO QUE OHM)
// =====================================================
static float freq_read_voltage()
{
    // Permite liberar 3 pines RNGx para uso fuera MODE OHM
    rng_release_for_gpio();

    // Usamos el mismo canal/rango que OHM (ajusta si tienes uno específico)
    adc_manager_select(RANGE_OHM_100);

    float v = adc_manager_read_voltage();

    return v;
}

// =====================================================
// FRECUENCIA — RAW (sin calibrar)
// =====================================================
float measureFrequency_raw()
{
    const unsigned long TIMEOUT = 200000; // 200 ms
    const float TH = 0.5f;

    unsigned long tStart = micros();

    // Esperar flanco ascendente inicial (baja -> alta)
    while (freq_read_voltage() < TH)
    {
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    }
    while (freq_read_voltage() > TH)
    {
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    }

    // Primer flanco ascendente
    while (freq_read_voltage() < TH)
    {
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    }
    unsigned long t1 = micros();

    // Segundo flanco ascendente
    while (freq_read_voltage() > TH)
    {
        if (micros() - t1 > TIMEOUT)
            return 0.0f;
    }
    while (freq_read_voltage() < TH)
    {
        if (micros() - t1 > TIMEOUT)
            return 0.0f;
    }
    unsigned long t2 = micros();

    float period_us = (float)(t2 - t1);
    if (period_us <= 0.0f)
        return 0.0f;

    return 1e6f / period_us;
}

// =====================================================
// FRECUENCIA — CALIBRADA
// =====================================================
float measureFrequency_calibrated()
{
    float f = measureFrequency_raw();
    if (f <= 0.0f)
        return 0.0f;
    return f * cal.freq_factor;
}

// =====================================================
// DUTY CYCLE (%)
// =====================================================
float measureDutyCycle()
{
    const unsigned long TIMEOUT = 200000;
    const float TH = 0.5f;

    unsigned long tStart = micros();

    // Esperar flanco ascendente inicial
    while (freq_read_voltage() < TH)
    {
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    }
    while (freq_read_voltage() > TH)
    {
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    }

    // Flanco ascendente
    while (freq_read_voltage() < TH)
    {
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    }
    unsigned long tRise = micros();

    // Flanco descendente
    while (freq_read_voltage() > TH)
    {
        if (micros() - tRise > TIMEOUT)
            return 0.0f;
    }
    unsigned long tFall = micros();

    // Segundo flanco ascendente
    while (freq_read_voltage() < TH)
    {
        if (micros() - tRise > TIMEOUT)
            return 0.0f;
    }
    unsigned long tNextRise = micros();

    float high_us = (float)(tFall - tRise);
    float period_us = (float)(tNextRise - tRise);

    if (period_us <= 0.0f)
        return 0.0f;

    return (high_us / period_us) * 100.0f;
}

// =====================================================
// PULSE WIDTH (HIGH time, us)
// =====================================================
float measurePulseWidth()
{
    const unsigned long TIMEOUT = 200000;
    const float TH = 0.5f;

    unsigned long tStart = micros();

    // Esperar flanco ascendente
    while (freq_read_voltage() < TH)
    {
        if (micros() - tStart > TIMEOUT)
            return 0.0f;
    }
    unsigned long tRise = micros();

    // Esperar flanco descendente
    while (freq_read_voltage() > TH)
    {
        if (micros() - tRise > TIMEOUT)
            return 0.0f;
    }
    unsigned long tFall = micros();

    return (float)(tFall - tRise);
}

// =====================================================
// PERIODO (us) — CALIBRADO
// =====================================================
float measurePeriod()
{
    float f = measureFrequency_calibrated();
    if (f <= 0.0f)
        return 0.0f;
    return 1e6f / f;
}

// =====================================================
// PANTALLAS
// =====================================================
void showFrequency()
{
    float f = measureFrequency_calibrated();

    if (f > 0.0f)
    {
        backlight_activity();
        autoOff_activity();
    }

    if (autoHold_update(f))
        f = autoHold_getHeldValue();

    lcd_ui_clear();
    lcd_ui_print("FREQ:");
    lcd_ui_printFloat(f, 1);
    lcd_ui_print(" Hz");
}

void showDutyCycle()
{
    float d = measureDutyCycle();

    if (d > 0.0f)
    {
        backlight_activity();
        autoOff_activity();
    }

    lcd_ui_clear();
    lcd_ui_print("DUTY:");
    lcd_ui_printFloat(d, 1);
    lcd_ui_print(" %");
}

void showPulseWidth()
{
    float pw = measurePulseWidth();

    if (pw > 0.0f)
    {
        backlight_activity();
        autoOff_activity();
    }

    lcd_ui_clear();
    lcd_ui_print("PW:");
    lcd_ui_printFloat(pw, 1);
    lcd_ui_print(" us");
}

void showPeriod()
{
    float p = measurePeriod();

    if (p > 0.0f)
    {
        backlight_activity();
        autoOff_activity();
    }

    lcd_ui_clear();
    lcd_ui_print("PER:");
    lcd_ui_printFloat(p, 1);
    lcd_ui_print(" us");
}

// =====================================================
// MODO COMPLETO
// =====================================================
void measureFREQ()
{
    backlight_activity();
    autoOff_activity();

    // Máxima velocidad del ADS para este modo
    adc_manager_set_sps(ADC_SPS_860);

    switch (freqSubMode)
    {
    case FREQ_MAIN:
        showFrequency();
        break;
    case FREQ_DUTY:
        showDutyCycle();
        break;
    case FREQ_PULSE_WIDTH:
        showPulseWidth();
        break;
    case FREQ_PERIOD:
        showPeriod();
        break;
    }
}