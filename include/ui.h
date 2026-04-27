#ifndef UI_H
#define UI_H

#include <lvgl.h>

void ui_init();
void ui_set_time(const char * time_str);
void ui_update_vpet();
void ui_update_steps();
void ui_set_sleep_mode(bool sleep); // Nueva

#endif