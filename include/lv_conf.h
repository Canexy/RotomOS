#if 1
#ifndef LV_CONF_H
#define LV_CONF_H
#include <stdint.h>

// ── MEMORIA ────────────────────────────────────────────────────────────────
// En ESP32 el buffer de display ya va a PSRAM (ps_malloc en hal.cpp).
// Esta memoria es para objetos LVGL internos: animaciones, estilos, etc.
// Con 466×466 y animaciones de sprites, 256KB es el mínimo razonable.
#define LV_MEM_CUSTOM  0
#define LV_MEM_SIZE    (256U * 1024U)   // 256 KB (antes: 128KB, insuficiente)
#define LV_MEM_ADR     0

// ── DISPLAY ────────────────────────────────────────────────────────────────
#define LV_DISP_DEF_REFR_PERIOD  16    // ~60 FPS
#define LV_INDEV_DEF_READ_PERIOD 16

// ── TICK ───────────────────────────────────────────────────────────────────
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #ifdef ESP32
        #define LV_TICK_CUSTOM_INCLUDE       "Arduino.h"
        #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
    #else
        #define LV_TICK_CUSTOM_INCLUDE       <SDL2/SDL.h>
        #define LV_TICK_CUSTOM_SYS_TIME_EXPR (SDL_GetTicks())
    #endif
#endif

// ── FUENTES ────────────────────────────────────────────────────────────────
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_48 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

// ── COLOR ──────────────────────────────────────────────────────────────────
#define LV_COLOR_DEPTH   16
#define LV_COLOR_16_SWAP  0

// ── FEATURES ──────────────────────────────────────────────────────────────
// Activa si quieres el GIF decoder de LVGL en el futuro
#define LV_USE_GIF       0
// Activa para imágenes PNG desde SD (Fase 2)
#define LV_USE_PNG       0

#endif /* LV_CONF_H */
#endif