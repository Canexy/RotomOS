#ifndef UI_H
#define UI_H

#include <lvgl.h>

void ui_init();
void ui_set_time(const char * time_str);
void ui_update_vpet();
void ui_update_steps();
void ui_set_sleep_mode(bool sleep);
void ui_update_pkm_image(); // Nueva función para la evolución
LV_FONT_DECLARE(lv_font_montserrat_12_lat);
LV_FONT_DECLARE(lv_font_montserrat_14_lat);
LV_FONT_DECLARE(lv_font_montserrat_16_lat);
LV_FONT_DECLARE(lv_font_montserrat_32_lat);
LV_FONT_DECLARE(lv_font_montserrat_48_lat);

#endif