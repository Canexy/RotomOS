#ifndef UI_H
#define UI_H

#include <lvgl.h>

void ui_init();
void ui_set_time(const char * time_str);
void ui_update_vpet(); // Nueva función para refrescar stats de la mascota
void ui_update_steps(); // Nueva función para refrescar pasos

#endif