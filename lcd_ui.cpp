#include "lcd_ui.h"
#include "globals.h"

// =====================================================
// FUNCIONES BÁSICAS DE UI
// =====================================================

void lcd_ui_clear()
{
    lcd.clear();
}

void lcd_ui_print(const char *text)
{
    lcd.print(text);
}

void lcd_ui_printFloat(float value, uint8_t decimals)
{
    lcd.print(value, decimals);
}

void lcd_ui_setCursor(uint8_t col, uint8_t row)
{
    lcd.setCursor(col, row);
}

// =====================================================
// BACKLIGHT
// =====================================================

void lcd_ui_backlightOn()
{
    lcd.backlight();
}

void lcd_ui_backlightOff()
{
    lcd.noBacklight();
}