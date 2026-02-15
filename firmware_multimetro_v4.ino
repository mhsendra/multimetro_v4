#include "globals.h"

// Prototipos
void loadCalibration();
void adc_manager_init();
void backlight_update();
void autoOff_update();

void setup()
{
    Serial.begin(115200);

    // Inicializar bus I2C primero
    Wire.begin();

    // LCD
    lcd_driver_init(&lcd, I2C_ADDR_LCD, LCD_COLS, LCD_ROWS);
    lcd_driver_clear(&lcd);

    // Expanders
    pcfExpander.begin();
    mcpExpander.begin();

    // Configurar pines del selector usando los wrappers IOExpander
    pcfExpander.pinMode(pin.SEL0, INPUT_PULLUP);
    pcfExpander.pinMode(pin.SEL1, INPUT_PULLUP);
    pcfExpander.pinMode(pin.SEL2, INPUT_PULLUP);

    // Registrar pines en el selector (usando IOExpander*)
    selector.addPin(&pcfExpander, pin.SEL0);
    selector.addPin(&pcfExpander, pin.SEL1);
    selector.addPin(&pcfExpander, pin.SEL2);

    // Mapear valores de selector a modos
    uint8_t fallbackMode = MODE_VDC; // variable para fallback
    selector.addMapping(0b000, MODE_OFF);
    selector.addMapping(0b001, MODE_VDC);
    selector.addMapping(0b010, MODE_VAC);
    selector.addMapping(0b011, MODE_CURRENT);
    selector.addMapping(0b100, MODE_OHM);

    // Inicializar calibración y ADC
    loadCalibration();
    adc_manager_init();

    // Inicializar estados de backlight y auto-off
    lastActivityTime = millis();
    lastBacklightActivity = millis();
    autoOffActive = false;
    backlightOff = false;

    // Limpiar LCD
    lcd_driver_clear(&lcd);
}

void loop()
{
    // Variable fallback
    uint8_t fallbackMode = MODE_VDC;

    // Variables auxiliares para updateMode
    uint8_t selectedModeVal = (uint8_t)selectedMode;
    uint8_t currentRangeVal = (uint8_t)currentRange;

    // Actualizar modo y rango desde selector
    selector.updateMode(selectedModeVal, currentRangeVal, fallbackMode);

    // Guardar cambios de vuelta a las enums
    selectedMode = (MainMode)selectedModeVal;
    currentRange = (CurrentRange)currentRangeVal;

    // Aquí colocar el resto de la lógica principal del multímetro
    // Lecturas ADC, filtros, mediciones, etc.

    // Actualizar backlight y auto-off
    backlight_update();
    autoOff_update();
}