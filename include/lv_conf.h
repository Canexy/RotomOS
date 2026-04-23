/* Cambiamos el 0 por 1 para "encender" la configuración */
#if 1 

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* --- CONFIGURACIÓN DE MEMORIA (Para tu S3) --- */
#define LV_MEM_CUSTOM      0
#define LV_MEM_SIZE        (128U * 1024U) // Usamos 128KB de RAM para empezar
#define LV_MEM_ADR         0

/* --- CONFIGURACIÓN DE PANTALLA --- */
#define LV_DISP_DEF_REFR_PERIOD  16      // 16ms = ~60 FPS (Lo que buscamos)
#define LV_INDEV_DEF_READ_PERIOD 16      // Frecuencia del táctil

/* --- TICK DEL SISTEMA --- */
// Importante para que las animaciones se muevan
/* --- TICK DEL SISTEMA (Reloj de LVGL) --- */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #ifdef ESP32
        // Si estamos en el reloj real, usamos Arduino
        #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
        #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
    #else
        // Si estamos en el PC, usamos el reloj de SDL2
        #define LV_TICK_CUSTOM_INCLUDE <SDL2/SDL.h>
        #define LV_TICK_CUSTOM_SYS_TIME_EXPR (SDL_GetTicks())
    #endif
#endif

/* --- FUENTES (Para los textos del VPet) --- */
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_32 1  // <--- Activa esta
#define LV_FONT_MONTSERRAT_48 1  // <--- Activa esta
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* --- OTROS --- */
#define LV_COLOR_DEPTH     16     // 16 bits para tus 65K colores
#define LV_COLOR_16_SWAP   0      // Si los colores salen raros (azul por rojo), cambiaremos esto a 1

#endif /*LV_CONF_H*/
#endif /*End of "at the beginning of the file"*/