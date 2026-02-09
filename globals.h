#ifndef GLOBALS_H
#define GLOBALS_H

#include "pins.h"
#include "config.h"
#include <Adafruit_ADS1X15.h>
#include <LiquidCrystal_I2C.h>
#include "filters.h"

// === Pines ===
extern Pins pin; // Struct de pines
extern PCF pcf;  // Struct de pines del PCF8574

// === LCD ===
extern LiquidCrystal_I2C lcd;

// === Filtros ===
extern Butterworth2 bw_vdc;
void initFilters();

// === Modos y submodos ===
extern MainMode selectedMode;
extern DiodeSubMode diodeSubMode;
extern CapSubMode capSubMode;
extern FreqSubMode freqSubMode;
extern OhmSubMode ohmSubMode;
extern VdcSubMode vdcSubMode;
extern VacSubMode vacSubMode;
extern OhmRange currentOhmRange;
extern CurrentRange currentRange;

// === Rango VDC ===
extern float vdc_ranges[3];
extern int vdc_range;
extern float acsOffset;

// === Calibración ===
extern Calibration cal;

// === Cable test ===
extern bool cableOK;
extern unsigned long lastBreak;

// === Auto-off y backlight ===
extern unsigned long lastActivityTime;
extern bool autoOffActive;
extern unsigned long lastBacklightActivity;
extern bool backlightOff;

// === Medidas y referencias ===
extern float vdcRef;
extern float ohmRef;
extern float ohmMin;
extern float ohmMax;

// === ADC ===
extern Adafruit_ADS1115 ads;
extern uint16_t ads_mux;
extern uint16_t ads_gain;
extern uint16_t ads_sps;

// Estado actual del PCF8574 (para restaurar si hace falta)
extern uint8_t matrix_pcf_state;

#endif