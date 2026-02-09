#include "menus.h"
#include "lcd_ui.h"
#include "globals.h"

/* =====================================================
 * MENÚ FRECUENCIA
 * ===================================================== */
void freqMenu()
{
    lcd_ui_clear(&lcd);
    lcd_printAt(&lcd, 0, 0, "FREQ MENU");

    switch (freqSubMode)
    {
    case FREQ_MAIN:
        lcd_printAt(&lcd, 0, 1, "1) Frequency");
        break;
    case FREQ_DUTY:
        lcd_printAt(&lcd, 0, 1, "2) Duty");
        break;
    case FREQ_PULSE_WIDTH:
        lcd_printAt(&lcd, 0, 1, "3) PulseWidth");
        break;
    case FREQ_PERIOD:
        lcd_printAt(&lcd, 0, 1, "4) Period");
        break;
    }
}

/* =====================================================
 * MENÚ VDC
 * ===================================================== */
void vdcMenu()
{
    lcd_ui_clear(&lcd);
    lcd_printAt(&lcd, 0, 0, "VDC MENU");

    switch (vdcSubMode)
    {
    case VDC_MAIN:
        lcd_printAt(&lcd, 0, 1, "1) Voltage");
        break;
    case VDC_POWER:
        lcd_printAt(&lcd, 0, 1, "2) Power");
        break;
    case VDC_ENERGY:
        lcd_printAt(&lcd, 0, 1, "3) Energy");
        break;
    case VDC_CURRENT_EST:
        lcd_printAt(&lcd, 0, 1, "4) Est Curr");
        break;
    case VDC_REL:
        lcd_printAt(&lcd, 0, 1, "5) Rel");
        break;
    }
}

/* =====================================================
 * MENÚ VAC
 * ===================================================== */
void vacMenu()
{
    lcd_ui_clear(&lcd);
    lcd_printAt(&lcd, 0, 0, "VAC MENU");

    switch (vacSubMode)
    {
    case VAC_MAIN:
        lcd_printAt(&lcd, 0, 1, "1) RMS");
        break;
    case VAC_REL:
        lcd_printAt(&lcd, 0, 1, "2) Rel");
        break;
    }
}

/* =====================================================
 * MENÚ OHM
 * ===================================================== */
void ohmMenu()
{
    lcd_ui_clear(&lcd);
    lcd_printAt(&lcd, 0, 0, "OHM MENU");

    switch (ohmSubMode)
    {
    case OHM_MAIN:
        lcd_printAt(&lcd, 0, 1, "1) R");
        break;
    case OHM_CONT:
        lcd_printAt(&lcd, 0, 1, "2) Cont");
        break;
    case OHM_REL:
        lcd_printAt(&lcd, 0, 1, "3) Rel");
        break;
    case OHM_MINMAX:
        lcd_printAt(&lcd, 0, 1, "4) Min/Max");
        break;
    case OHM_CABLE:
        lcd_printAt(&lcd, 0, 1, "5) Cable");
        break;
    }
}

/* =====================================================
 * MENÚ DIODO
 * ===================================================== */
void diodeMenu()
{
    lcd_ui_clear(&lcd);
    lcd_printAt(&lcd, 0, 0, "DIODE MENU");

    switch (diodeSubMode)
    {
    case DIODE_MAIN:
        lcd_printAt(&lcd, 0, 1, "1) Diodo");
        break;
    case DIODE_TRANSISTOR:
        lcd_printAt(&lcd, 0, 1, "2) Trans");
        break;
    case DIODE_MOSFET:
        lcd_printAt(&lcd, 0, 1, "3) MOSFET");
        break;
    case DIODE_ZENER:
        lcd_printAt(&lcd, 0, 1, "4) Zener");
        break;
    case DIODE_AUTO:
        lcd_printAt(&lcd, 0, 1, "5) Auto");
        break;
    }
}

int readSelector()
{
    int b0 = digitalRead(pin.SEL0);
    int b1 = digitalRead(pin.SEL1);
    int b2 = digitalRead(pin.SEL2);

    return (b2 << 2) | (b1 << 1) | b0;
}
