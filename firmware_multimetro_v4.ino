#include "config.h"
#include "pins.h"
#include "filters.h"
#include "globals.h"
#include "lcd_ui.h"
#include "menus.h"
#include "calibration.h"
#include "adcmanager.h"
#include "mode_vdc.h"
#include "mode_vac.h"
#include "mode_ohm.h"
#include "mode_current.h"
#include "mode_cap.h"
#include "mode_freq.h"
#include "mode_diode.h"
#include "mode_zener.h"
#include "induct.h"
#include "AutoOff.h"
#include "lcd_setup.h"
#include "backlight.h"
#include "mode_PN.h"

void setup()
{
    pinMode(pin.PIN_ONOFF, OUTPUT);
    digitalWrite(pin.PIN_ONOFF, HIGH); // mantener encendido

    backlight_reset();
    // Inicialización de hardware
    Wire.begin();
    lcd_setup();

    // Inicialización de pines
    pinMode(pin.PIN_CAL, INPUT_PULLUP);
    pinMode(pin.PIN_BUZZER, OUTPUT);
    pinMode(pin.SEL0, INPUT_PULLUP);
    pinMode(pin.SEL1, INPUT_PULLUP);
    pinMode(pin.SEL2, INPUT_PULLUP);

    // Inicialización de módulos
    autoOff_reset();
    loadCalibration();
    initFilters();
    adc_manager_init();
    lcd.clear();
    lcd.print("Multímetro");
    delay(1000);
}

void loop()
{
    backlight_update();
    autoOff_update();

    // Leer selector
    int mode = readSelector();

    // Ejecutar modo
    switch (mode)
    {

    case MODE_VDC:
        backlight_activity();
        autoOff_activity();
        measureVDC_MODE();
        break;

    case MODE_VAC:
        backlight_activity();
        autoOff_activity();
        measureVAC_MODE();
        break;

    case MODE_OHM_CONT:
        backlight_activity();
        autoOff_activity();
        measureOHM_MODE(); // dispatcher real del modo ohm
        break;

    case MODE_DIODE_TRANS:
        backlight_activity();
        autoOff_activity();
        measurePN_MODE(); // dispatcher real del modo diodo
        break;

    case MODE_CURR_mA:
        backlight_activity();
        autoOff_activity();
        currentRange = CURR_RANGE_mA;
        measureCURRENT();
        break;

    case MODE_CURR_A:
        backlight_activity();
        autoOff_activity();
        currentRange = CURR_RANGE_5A;
        measureCURRENT();
        break;

    case MODE_CAP:
        backlight_activity();
        autoOff_activity();
        measureCAPMode();
        break;

    case MODE_L:
        backlight_activity();
        autoOff_activity();
        void measureInductance();
        break;

    default:
        lcd.clear();
        lcd.print("Modo invalido");
        break;
    }

    delay(100); // estabilidad
}