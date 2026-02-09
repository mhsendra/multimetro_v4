#include <Arduino.h>
#include <math.h>
#include "Mode_CAP.h"
#include "adcmanager.h"
#include "lcd_ui.h"
#include "globals.h"
#include "config.h"
#include "cap_utils.h"
#include "auto_hold.h"
#include "backlight.h"
#include "autoOff.h"
#include "mode_current.h"
#include "range_control.h"

// =====================================================
// PROTECCIÓN POR VOLTAJE RESIDUAL
// =====================================================
static bool capResidualVoltageTooHigh()
{
    adc_manager_select(RANGE_DC_20V);
    int16_t raw = 0;
    if (!adc_manager_read_raw(&raw))
        return true;

    float v_adc = adc_manager_read_voltage();

    // Escala del divisor para DC_20V
    float v = v_adc * 0.110f;

    return v > CAP_RESIDUAL_VOLT_MAX;
}

// =====================================================
// MEDICIÓN DE CAPACITANCIA
// =====================================================
float measureCapacitance()
{
    backlight_activity();
    autoOff_activity();

    // 1. Protección por voltaje residual
    if (capResidualVoltageTooHigh())
        return -1;

    // 2. Descargar condensador
    dischargeCap();

    // 3. Preparar carga
    pinMode(pin.CAP_CHARGE_PIN, OUTPUT);
    digitalWrite(pin.CAP_CHARGE_PIN, HIGH);

    // 4. Seleccionar rango ADS
    adc_manager_select(RANGE_DC_2V);

    // 5. Medir tiempo hasta alcanzar el umbral
    unsigned long tStart = micros();

    while (true)
    {
        int16_t raw = 0;
        if (!adc_manager_read_raw(&raw))
            return -3; // error lectura

        float v = adc_manager_read_voltage();

        if (v >= CAP_THRESHOLD)
            break;

        if (micros() - tStart > CAP_TIMEOUT_US)
            return -2; // timeout
    }

    unsigned long dt = micros() - tStart;

    // 6. Obtener resistencia según submodo
    float R = CAP_R_UF;
    switch (capSubMode)
    {
    case CAP_RANGE_NF:
        R = CAP_R_NF;
        break;
    case CAP_RANGE_UF:
        R = CAP_R_UF;
        break;
    case CAP_RANGE_MF:
        R = CAP_R_MF;
        break;
    }

    // 7. Calcular capacitancia
    float C = (float)dt / (R * 1e6);

    if (C > 0 && !isnan(C) && !isinf(C))
    {
        backlight_activity();
        autoOff_activity();
    }

    return C;
}

// =====================================================
// ESR RAW
// =====================================================
float measureESR_raw()
{
    float i = measureCURRENT_calibrated();

    if (i < 1e-5f)
        return INFINITY;

    return 5.0f / i; // ESR = V / I
}

// =====================================================
// ESR CALIBRADO
// =====================================================
float measureESR()
{
    float esr = measureESR_raw();
    if (isnan(esr) || isinf(esr))
        return esr;

    return esr * cal.esr_factor;
}

// =====================================================
// PANTALLA CAPACITANCIA
// =====================================================
void showCapacitance()
{
    backlight_activity();
    autoHold_reset();
    autoOff_reset();

    lcd_ui_clear();
    lcd_ui_print("Detectando...");
    delay(200);

    float C = measureCapacitance();

    if (!isnan(C) && !isinf(C) && C > 0)
    {
        backlight_activity();
        autoOff_activity();
    }

    // --- AUTO HOLD ---
    if (autoHold_update(C))
    {
        float held = autoHold_getHeldValue();
        lcd_ui_clear();
        lcd_ui_print("CAP (HOLD)");
        lcd_ui_setCursor(0, 1);

        if (held == -1)
        {
            lcd_ui_print("RESIDUAL");
            return;
        }
        if (held == -2)
        {
            lcd_ui_print("TIMEOUT");
            return;
        }

        if (capSubMode == CAP_RANGE_NF)
            lcd_ui_printFloat(held * 1e9, 1), lcd_ui_print(" nF");
        else if (capSubMode == CAP_RANGE_UF)
            lcd_ui_printFloat(held * 1e6, 1), lcd_ui_print(" uF");
        else
            lcd_ui_printFloat(held * 1e3, 2), lcd_ui_print(" mF");

        return;
    }

    // --- LECTURA NORMAL ---
    lcd_ui_clear();

    if (C == -1)
    {
        lcd_ui_print("RESIDUAL VOLT");
        return;
    }

    if (C == -2)
    {
        lcd_ui_print("TIMEOUT");
        return;
    }

    if (capSubMode == CAP_RANGE_NF)
        lcd_ui_printFloat(C * 1e9, 1), lcd_ui_print(" nF");
    else if (capSubMode == CAP_RANGE_UF)
        lcd_ui_printFloat(C * 1e6, 1), lcd_ui_print(" uF");
    else
        lcd_ui_printFloat(C * 1e3, 2), lcd_ui_print(" mF");
}

// =====================================================
// PANTALLA ESR
// =====================================================
void showESR()
{
    backlight_activity();
    autoHold_reset();
    autoOff_reset();

    lcd_ui_clear();
    lcd_ui_print("Detectando...");
    delay(200);

    adc_manager_set_sps(ADC_SPS_860);

    float esr = measureESR();

    if (!isnan(esr) && esr != INFINITY)
    {
        backlight_activity();
        autoOff_activity();
    }

    // --- AUTO HOLD ---
    if (autoHold_update(esr))
    {
        float held = autoHold_getHeldValue();
        lcd_ui_clear();
        lcd_ui_print("ESR (HOLD)");
        lcd_ui_setCursor(0, 1);

        if (held == INFINITY)
        {
            lcd_ui_print("OPEN");
            return;
        }
        if (isnan(held) || held < 0)
        {
            lcd_ui_print("ERROR");
            return;
        }

        lcd_ui_printFloat(held, 2);
        lcd_ui_print(" Ohm");
        return;
    }

    // --- LECTURA NORMAL ---
    lcd_ui_clear();
    lcd_ui_print("ESR:");
    lcd_ui_setCursor(0, 1);

    if (esr == INFINITY)
        lcd_ui_print("OPEN");
    else if (isnan(esr) || esr < 0)
        lcd_ui_print("ERROR");
    else
        lcd_ui_printFloat(esr, 2), lcd_ui_print(" Ohm");
}

// =====================================================
// API PÚBLICA DEL MODO CAP
// =====================================================
void measureCAPMode()
{
    rng_release_for_gpio();

    switch (capSubMode)
    {
    case CAP_RANGE_NF:
    case CAP_RANGE_UF:
    case CAP_RANGE_MF:
        showCapacitance();
        break;

    case CAP_ESR:
        showESR();
        break;
    }
}