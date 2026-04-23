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

// Inicializa TODO el hardware (Pantalla, LVGL, Memoria)
void hal_setup();

// Gestiona los eventos del sistema (Cerrar ventana en PC, etc)
void hal_loop();

#endif