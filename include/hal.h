#ifndef HAL_H
#define HAL_H

#include <lvgl.h>

#ifdef ESP32
    #include <Arduino.h>
    #include "LGFX_Config.h"
    extern LGFX tft;
#else
    #define SDL_MAIN_HANDLED
    #include <SDL2/SDL.h>
#endif

void hal_setup();
void hal_loop();
void hal_get_time(int *h, int *m);

// --- Nuevas funciones de sensores ---
uint32_t hal_get_steps();
int hal_get_battery();
void hal_update_sensors(); // Se llama en cada loop para procesar hardware

#endif