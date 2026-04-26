#include "ui.h"
#include "system_data.h"

lv_obj_t * tv;
lv_obj_t * tile_clock;
lv_obj_t * tile_pet;
lv_obj_t * tile_stats;

lv_obj_t * time_label;
lv_obj_t * level_label;
lv_obj_t * hunger_bar;
lv_obj_t * happy_bar;
lv_obj_t * steps_label;
lv_obj_t * batt_label;

LV_IMG_DECLARE(rotom_bg); 

void ui_set_time(const char * time_str) {
    if(time_label) lv_label_set_text(time_label, time_str);
}

void ui_update_vpet() {
    if(!level_label) return;
    
    lv_label_set_text_fmt(level_label, "Nivel %d", myStatus.level);
    lv_bar_set_value(hunger_bar, myStatus.hunger, LV_ANIM_ON);
    lv_bar_set_value(happy_bar, myStatus.happiness, LV_ANIM_ON);
    
    // Batería
    lv_label_set_text_fmt(batt_label, "%d%%", myStatus.battery);
    if(myStatus.battery < 20) lv_obj_set_style_text_color(batt_label, lv_color_make(255, 50, 50), 0);
    else lv_obj_set_style_text_color(batt_label, lv_color_white(), 0);
}

void ui_update_steps() {
    if(steps_label) lv_label_set_text_fmt(steps_label, "%d / %d", myStatus.steps, myStatus.step_goal);
}

void ui_init() {
    lv_obj_t * screen = lv_scr_act();
    
    // 1. FONDO
    lv_obj_t * main_bg = lv_img_create(screen);
    lv_img_set_src(main_bg, &rotom_bg);
    lv_obj_center(main_bg);
    lv_obj_clear_flag(main_bg, LV_OBJ_FLAG_SCROLLABLE); 

    // 2. INDICADOR BATERÍA (Siempre visible arriba)
    batt_label = lv_label_create(screen);
    lv_obj_align(batt_label, LV_ALIGN_TOP_RIGHT, -45, 45);
    lv_label_set_text(batt_label, "---%");

    // 3. TILEVIEW
    tv = lv_tileview_create(screen);
    lv_obj_set_size(tv, 466, 466);
    lv_obj_set_style_bg_opa(tv, 0, 0); 
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    tile_clock = lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR);
    tile_pet   = lv_tileview_add_tile(tv, 1, 0, LV_DIR_HOR);
    tile_stats = lv_tileview_add_tile(tv, 2, 0, LV_DIR_HOR);

    // --- TILE 1: RELOJ ---
    time_label = lv_label_create(tile_clock);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

    // --- TILE 2: CARA POKÉMON ---
    lv_obj_t * pkm_name = lv_label_create(tile_pet);
    lv_label_set_text(pkm_name, myStatus.pkm_name);
    lv_obj_set_style_text_font(pkm_name, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(pkm_name, lv_color_make(255, 215, 0), 0);
    lv_obj_align(pkm_name, LV_ALIGN_TOP_MID, 0, 90);

    level_label = lv_label_create(tile_pet);
    lv_obj_align(level_label, LV_ALIGN_TOP_MID, 0, 130);
    lv_obj_set_style_text_color(level_label, lv_color_white(), 0);

    lv_obj_t * pkm_circle = lv_obj_create(tile_pet);
    lv_obj_set_size(pkm_circle, 180, 180);
    lv_obj_set_style_radius(pkm_circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_center(pkm_circle);
    lv_obj_set_style_bg_color(pkm_circle, lv_color_make(60, 60, 60), 0);
    lv_obj_set_style_border_width(pkm_circle, 4, 0);
    lv_obj_set_style_border_color(pkm_circle, lv_color_make(255, 215, 0), 0);

    // --- TILE 3: STATS ---
    lv_obj_t * cont = lv_obj_create(tile_stats);
    lv_obj_set_size(cont, 320, 320);
    lv_obj_center(cont);
    lv_obj_set_style_bg_opa(cont, 180, 0);
    lv_obj_set_style_radius(cont, 20, 0);

    lv_obj_t * h_title = lv_label_create(cont);
    lv_label_set_text(h_title, "HAMBRE");
    lv_obj_align(h_title, LV_ALIGN_TOP_LEFT, 10, 10);
    hunger_bar = lv_bar_create(cont);
    lv_obj_set_size(hunger_bar, 240, 20);
    lv_obj_align(hunger_bar, LV_ALIGN_TOP_LEFT, 10, 35);
    lv_obj_set_style_bg_color(hunger_bar, lv_color_make(255, 0, 0), LV_PART_INDICATOR);

    lv_obj_t * f_title = lv_label_create(cont);
    lv_label_set_text(f_title, "AMISTAD");
    lv_obj_align(f_title, LV_ALIGN_TOP_LEFT, 10, 80);
    happy_bar = lv_bar_create(cont);
    lv_obj_set_size(happy_bar, 240, 20);
    lv_obj_align(happy_bar, LV_ALIGN_TOP_LEFT, 10, 105);
    lv_obj_set_style_bg_color(happy_bar, lv_color_make(0, 255, 0), LV_PART_INDICATOR);

    steps_label = lv_label_create(cont);
    lv_obj_align(steps_label, LV_ALIGN_TOP_LEFT, 10, 160);
    lv_obj_set_style_text_font(steps_label, &lv_font_montserrat_16, 0);

    ui_update_vpet();
    ui_update_steps();
}