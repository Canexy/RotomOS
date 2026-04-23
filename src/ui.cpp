#include "ui.h"
#include "system_data.h"

lv_obj_t * tv;
lv_obj_t * tile_clock;
lv_obj_t * tile_steps;
lv_obj_t * time_label;

LV_IMG_DECLARE(rotom_bg); 

void ui_set_time(const char * time_str) {
    if(time_label) lv_label_set_text(time_label, time_str);
}

void ui_init() {
    lv_obj_t * screen = lv_scr_act();
    
    // --- 1. EL FONDO "STICKY" (466x466) ---
    lv_obj_t * main_bg = lv_img_create(screen);
    lv_img_set_src(main_bg, &rotom_bg);
    lv_obj_center(main_bg);

    // BLOQUEO ABSOLUTO: Ni se mueve, ni se toca, ni hace scroll
    lv_obj_clear_flag(main_bg, LV_OBJ_FLAG_SCROLLABLE); 
    lv_obj_clear_flag(main_bg, LV_OBJ_FLAG_CLICKABLE); 

    // --- 2. TILEVIEW (Transparente encima del fondo) ---
    tv = lv_tileview_create(screen);
    lv_obj_set_size(tv, 466, 466);
    lv_obj_center(tv);
    lv_obj_set_style_bg_opa(tv, 0, 0); 
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    tile_clock = lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR);
    tile_steps = lv_tileview_add_tile(tv, 1, 0, LV_DIR_HOR);

    // --- PANTALLA 1: RELOJ ---
    time_label = lv_label_create(tile_clock);
    lv_label_set_text(time_label, "--:--");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0); 
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0); 

    // --- PANTALLA 2: PASOS + BOTÓN ---
    lv_obj_t * stp_val = lv_label_create(tile_steps);
    lv_label_set_text(stp_val, "0"); 
    lv_obj_set_style_text_font(stp_val, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(stp_val, lv_color_white(), 0);
    lv_obj_align(stp_val, LV_ALIGN_CENTER, 0, -40);

    lv_obj_t * btn = lv_btn_create(tile_steps);
    lv_obj_set_size(btn, 220, 70); 
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 80);

    lv_obj_set_style_bg_color(btn, lv_color_make(0, 100, 255), 0);
    lv_obj_set_style_radius(btn, 35, 0);
    
    // Estilo pulsado (Encogimiento)
    lv_obj_set_style_bg_color(btn, lv_color_make(0, 50, 150), LV_STATE_PRESSED);
    lv_obj_set_style_width(btn, 200, LV_STATE_PRESSED); 
    lv_obj_set_style_height(btn, 60, LV_STATE_PRESSED);

    lv_obj_t * btn_lbl = lv_label_create(btn);
    lv_label_set_text(btn_lbl, "POKEDEX");
    lv_obj_center(btn_lbl);
}