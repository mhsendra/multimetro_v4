#include "config.h"
#include "globals.h"
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
#include "mode_dispatchers.h"

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
    // 1️⃣ Ejecutar el modo seleccionado
    dispatchMode(selectedMode);

    // 2️⃣ Actualizaciones de sistemas auxiliares
    backlight_update();
    autoOff_update();

    // 3️⃣ Leer selector físico y actualizar selectedMode
    int sel = readSelector();

    switch (sel)
    {
    case 0:
        selectedMode = MODE_VDC;
        break;

    case 1:
        selectedMode = MODE_VAC;
        break;

    case 2:
        selectedMode = MODE_OHM;
        break;

    case 3:
        selectedMode = MODE_PN; // Diodo / Transistor / Zener / MOSFET
        break;

    case 4:
        selectedMode = MODE_CURRENT;
        currentRange = CURR_RANGE_mA; // rango miliamperios
        break;

    case 5:
        selectedMode = MODE_CURRENT;
        currentRange = CURR_RANGE_5A; // rango amperios
        break;

    case 6:
        selectedMode = MODE_CAP;
        break;

    case 7:
        selectedMode = MODE_INDUCT;
        break;

    default:
        selectedMode = MODE_VDC; // fallback seguro
        break;
    }

    // 4️⃣ Pequeña pausa para estabilidad
    delay(100);
}