#include "ui.h"
#include "system_data.h"

lv_obj_t * tv;
lv_obj_t * tile_clock;
lv_obj_t * tile_steps;

void ui_init() {
    lv_obj_t * screen = lv_scr_act();
    
    // 1. FONDO NEGRO TOTAL
    lv_obj_set_style_bg_color(screen, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_bg_opa(screen, 255, 0);

    // 2. TileView
    tv = lv_tileview_create(screen);
    lv_obj_set_size(tv, 240, 240);
    lv_obj_center(tv);
    lv_obj_set_style_bg_opa(tv, 0, 0); 
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    tile_clock = lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR);
    tile_steps = lv_tileview_add_tile(tv, 1, 0, LV_DIR_HOR);

    // --- PANTALLA 1: RELOJ ---
    lv_obj_t * time_label = lv_label_create(tile_clock);
    lv_label_set_text(time_label, "12:30");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(time_label, lv_color_make(255, 255, 255), 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -35);

    // --- EL BOTÓN POKÉMON ---
    lv_obj_t * btn = lv_btn_create(tile_clock);
    lv_obj_set_size(btn, 140, 50); 
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 45);

    // ESTILO NORMAL (Rojo con texto Negro)
    lv_obj_set_style_bg_color(btn, lv_color_make(220, 20, 20), 0);
    lv_obj_set_style_bg_opa(btn, 255, 0);
    lv_obj_set_style_radius(btn, 10, 0);
    lv_obj_set_style_shadow_opa(btn, 0, 0);
    // Texto negro de normal (se aplica al botón para que el hijo herede)
    lv_obj_set_style_text_color(btn, lv_color_make(255, 255, 255), 0);

    // ESTILO PULSADO (Rojo Oscuro, Letras Blancas y Encogido)
    lv_obj_set_style_bg_color(btn, lv_color_make(110, 0, 0), LV_STATE_PRESSED);
    lv_obj_set_style_text_color(btn, lv_color_make(255, 255, 255), LV_STATE_PRESSED);
    
    // Reducimos el tamaño (Manual)
    lv_obj_set_style_width(btn, 120, LV_STATE_PRESSED); 
    lv_obj_set_style_height(btn, 40, LV_STATE_PRESSED);

    // Forzamos a que no se mueva
    lv_obj_set_style_translate_x(btn, 0, LV_STATE_PRESSED); 
    lv_obj_set_style_translate_y(btn, 0, LV_STATE_PRESSED);

    lv_obj_t * btn_lbl = lv_label_create(btn);
    lv_label_set_text(btn_lbl, "Here!");
    lv_obj_center(btn_lbl);

    // --- PANTALLA 2: PASOS ---
    lv_obj_t * stp_title = lv_label_create(tile_steps);
    lv_label_set_text(stp_title, "Steps!");
    lv_obj_set_style_text_color(stp_title, lv_color_make(255, 255, 255), 0);
    lv_obj_align(stp_title, LV_ALIGN_TOP_MID, 0, 40);

    lv_obj_t * stp_val = lv_label_create(tile_steps);
    lv_label_set_text(stp_val, "0"); 
    lv_obj_set_style_text_font(stp_val, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(stp_val, lv_color_make(255, 255, 255), 0);
    lv_obj_center(stp_val);
}