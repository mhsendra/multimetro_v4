#include "globals.h"

// === Pines ===
Pins pin;
PCF pcf;

// === LCD ===
LiquidCrystal_I2C lcd(0x27, 16, 2);
// === Filtros ===
Butterworth2 bw_vdc;

// === Modos y submodos ===
MainMode selectedMode = MODE_VDC;
DiodeSubMode diodeSubMode = DIODE_MAIN;
CapSubMode capSubMode = CAP_RANGE_UF;
FreqSubMode freqSubMode = FREQ_MAIN;
OhmSubMode ohmSubMode = OHM_MAIN;
VdcSubMode vdcSubMode = VDC_MAIN;
VacSubMode vacSubMode = VAC_MAIN;
OhmRange currentOhmRange = OHM_RANGE_MID;
CurrentRange currentRange = CURR_RANGE_mA;

// === Rango VDC ===
float vdc_ranges[3] = {2.0f, 20.0f, 200.0f};
int vdc_range = 0;
float acsOffset = 0.0f;

// === Calibración ===
Calibration cal;

// === Cable test ===
bool cableOK = false;
unsigned long lastBreak = 0;

// === Auto-off y backlight ===
unsigned long lastActivityTime = 0;
bool autoOffActive = true;
unsigned long lastBacklightActivity = 0;
bool backlightOff = false;

// === Medidas y referencias ===
float vdcRef = 0.0f;
float ohmRef = NAN;
float ohmMin = 0.0f;
float ohmMax = 0.0f;

// === ADC ===
Adafruit_ADS1115 ads;
uint16_t ads_mux = 0;
uint16_t ads_gain = 0;
uint16_t ads_sps = 0;

// Estado actual del PCF8574 (para restaurar si hace falta)
uint8_t matrix_pcf_state = 0;