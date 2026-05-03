#pragma once
#include <lvgl.h>

#ifdef ESP32
    #include <Arduino.h>
    #include "LGFX_Config.h"
    extern LGFX tft;
#else
    #define SDL_MAIN_HANDLED
    #include <SDL2/SDL.h>
#endif

// Ciclo de vida
void hal_setup();
void hal_loop();
void hal_update_sensors();

// Tiempo (RTC en HW, sistema en emulador)
void hal_get_time(int *h, int *m, int *s);

// Sensores
uint32_t hal_get_steps();
int      hal_get_battery();   // 0–100%

// Utilidades
void     hal_set_brightness(uint8_t val); // 0–255