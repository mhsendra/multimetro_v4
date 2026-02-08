#include "calibration.h"
#include "adcmanager.h"
#include "mode_vac.h"
#include "mode_vdc.h"
#include "mode_ohm.h"
#include "mode_current.h"
#include "mode_freq.h"
#include "induct.h"
#include <EEPROM.h>
#include <Arduino.h>
#include "lcd_ui.h"
#include "pins.h"

// =====================================================
// CARGAR CALIBRACIÓN
// =====================================================
void loadCalibration()
{
    EEPROM.get(0, cal);

    if (isnan(cal.vdc))
    {
        cal.vdc = 1.0;
        cal.vac = 1.0;
        cal.ohm = 1.0;

        cal.curr_shunt_gain = 1.0;
        cal.curr_shunt_offset = 0.0;

        cal.acs_offset = 2.5;
        cal.acs_sens = 0.185; // valor típico ACS712-05B
    }
}

// =====================================================
// CALIBRACIONES INDIVIDUALES
// =====================================================
void calibrateVDC()
{
    lcd_ui_clear();
    lcd_ui_print("VDC: aplicar 5.00V");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_meas = measureVDC_calibrated();
    cal.vdc = 5.00 / v_meas;
}

void calibrateVAC()
{
    lcd_ui_clear();
    lcd_ui_print("VAC: aplicar 230V");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_meas = measureVAC_calibrated();
    cal.vac = 230.0 / v_meas;
}

void calibrateOHM()
{
    lcd_ui_clear();
    lcd_ui_print("OHM: conectar 1k");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float r_meas = measureOhmValue();
    cal.ohm = 1000.0 / r_meas;
}

void calibrateCurrent_mA()
{
    lcd_ui_clear();
    lcd_ui_print("I mA: 0mA");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    cal.curr_shunt_offset = measureCURRENT_calibrated(); // offset 0mA

    lcd_ui_clear();
    lcd_ui_print("I mA: 100mA");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_load = measureCURRENT_calibrated();
    float I_real = 0.100; // 100mA exactos
    cal.curr_shunt_gain = I_real / (v_load - cal.curr_shunt_offset);
}

void calibrateCurrent_5A()
{
    lcd_ui_clear();
    lcd_ui_print("I 5A: aplicar 5A");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_load = measureCURRENT_calibrated();
    float I_real = 5.00;
    cal.curr_shunt_gain = I_real / (v_load - cal.curr_shunt_offset);
}

void calibrateCurrent_16A()
{
    lcd_ui_clear();
    lcd_ui_print("ACS: 0A");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    cal.acs_offset = measureCurrent_ACS_RAW();

    lcd_ui_clear();
    lcd_ui_print("ACS: 10A");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float v_load = measureCurrent_ACS_RAW();
    float I_real = 10.0;
    cal.acs_sens = (v_load - cal.acs_offset) / I_real;
}

void calibrateESR()
{
    lcd_ui_clear();
    lcd_ui_print("ESR: corto");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float i0 = measureCURRENT_calibrated();
    if (i0 < 0.00001f)
        i0 = 0.00001f;

    cal.esr_factor = 5.0 / i0;
}

void calibrateFrequency()
{
    lcd_ui_clear();
    lcd_ui_print("FREQ: 1kHz");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float f_meas = measureFrequency_calibrated();
    if (f_meas < 1)
        f_meas = 1;

    cal.freq_factor = 1000.0 / f_meas;
}

void calibrateInductance()
{
    lcd_ui_clear();
    lcd_ui_print("IND: 1mH");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    float L_meas = measureInd_calibrated();

    if (L_meas < 0.000001f)
        L_meas = 0.000001f;

    cal.induct_factor = 0.001 / L_meas; // 1 mH exacto
}

// =====================================================
// MODO CALIBRACIÓN COMPLETO
// =====================================================
void enterCalibration()
{
    lcd_ui_clear();
    lcd_ui_print("CALIBRACION");
    delay(1500);

    // ============================
    // 1) OFFSET ADC
    // ============================
    lcd_ui_clear();
    lcd_ui_print("1) Desconectar entradas");
    lcd_ui_setCursor(0, 1);
    lcd_ui_print("OK=CAL");
    while (digitalRead(pin.PIN_CAL) == HIGH)
        ;
    delay(300);

    cal.vdc = 1.0;
    cal.vac = 1.0;
    cal.ohm = 1.0;
    cal.acs_offset = measureCurrent_ACS_RAW();

    // ============================
    // 2) VDC
    // ============================
    calibrateVDC();

    // ============================
    // 3) VAC
    // ============================
    calibrateVAC();

    // ============================
    // 4) OHMIOS
    // ============================
    calibrateOHM();

    // ============================
    // 5) CORRIENTE mA
    // ============================
    calibrateCurrent_mA();

    // ============================
    // 6) CORRIENTE 5A
    // ============================
    calibrateCurrent_5A();

    // ============================
    // 7) CORRIENTE 16A
    // ============================
    calibrateCurrent_16A();

    // ============================
    // 8) ESR
    // ============================
    calibrateESR();

    // ============================
    // 9) FRECUENCIA
    // ============================
    calibrateFrequency();

    // ============================
    // 10) INDUCTANCIA
    // ============================
    calibrateInductance();

    // ============================
    // Guardar en EEPROM
    // ============================
    EEPROM.put(0, cal);

    lcd_ui_clear();
    lcd_ui_print("CAL COMPLETA");
    delay(2000);
}