#include "globals.h"

void lcd_setup()
{
    lcd.begin(16, 2);
    lcd.backlight();
}