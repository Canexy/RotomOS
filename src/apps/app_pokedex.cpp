#include "app_manager.h"
#include "system_data.h"

LV_FONT_DECLARE(lv_font_montserrat_14_lat);
LV_FONT_DECLARE(lv_font_montserrat_48_lat);

#define C_BLACK lv_color_hex(0x000000)
#define C_TEXT  lv_color_hex(0xE8E4E0)
#define C_RED   lv_color_hex(0xCB3D2B)
#define C_DIM   lv_color_hex(0x5A6A76)

static lv_obj_t *scr = nullptr;
static void back_cb(lv_event_t *) { app_manager_exit(); }

void app_pokedex_launch() {
    scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, C_BLACK, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl = lv_label_create(scr);
    lv_label_set_text(lbl, "POKéDEX");
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_48_lat, 0);
    lv_obj_set_style_text_color(lbl, C_RED, 0);
    lv_obj_center(lbl);

    lv_obj_t *sub = lv_label_create(scr);
    lv_label_set_text(sub, "Proximamente");
    lv_obj_set_style_text_font(sub, &lv_font_montserrat_14_lat, 0);
    lv_obj_set_style_text_color(sub, C_DIM, 0);
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, 60);

    lv_obj_t *back = lv_btn_create(scr);
    lv_obj_set_size(back, 52, 52);
    lv_obj_set_style_radius(back, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(back, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_shadow_width(back, 0, 0);
    lv_obj_align(back, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_add_event_cb(back, back_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *bl = lv_label_create(back);
    lv_label_set_text(bl, "<");
    lv_obj_set_style_text_color(bl, C_TEXT, 0);
    lv_obj_set_style_text_font(bl, &lv_font_montserrat_14_lat, 0);
    lv_obj_center(bl);

    lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
}

void app_pokedex_destroy() {
    scr = nullptr;
}

void app_pokedex_tick() {}