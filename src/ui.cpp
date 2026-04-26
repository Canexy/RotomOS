#include "ui.h"
#include "system_data.h"

static lv_obj_t * tv;
static lv_obj_t * time_label, * level_label, * batt_label, * steps_label;
static lv_obj_t * hunger_bar, * happy_bar;

LV_IMG_DECLARE(rotom_bg); 
LV_IMG_DECLARE(pkm_sprite);

void ui_init() {
    lv_obj_t * screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Batería: Ahora en blanco como el reloj
    batt_label = lv_label_create(screen);
    lv_obj_set_style_text_color(batt_label, lv_color_white(), 0); 
    lv_obj_align(batt_label, LV_ALIGN_TOP_MID, 0, 30);

    // Tileview
    tv = lv_tileview_create(screen);
    lv_obj_set_size(tv, 466, 466);
    lv_obj_set_style_bg_opa(tv, 0, 0); 
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t * t1 = lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR);
    lv_obj_t * t2 = lv_tileview_add_tile(tv, 1, 0, LV_DIR_HOR);
    lv_obj_t * t3 = lv_tileview_add_tile(tv, 2, 0, LV_DIR_HOR);

    // --- PANTALLA 1: RELOJ ---
    time_label = lv_label_create(t1);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

    // --- PANTALLA 2: V-PET ---
    lv_obj_t * name_lbl = lv_label_create(t2);
    lv_label_set_text(name_lbl, myStatus.pkm_name);
    lv_obj_set_style_text_font(name_lbl, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(name_lbl, lv_color_make(255, 100, 0), 0); 
    lv_obj_align(name_lbl, LV_ALIGN_TOP_MID, 0, 70);

    level_label = lv_label_create(t2);
    lv_obj_set_style_text_color(level_label, lv_color_white(), 0);
    lv_obj_align(level_label, LV_ALIGN_TOP_MID, 0, 110);

    // Sprite de Cosmog (240x240)
    lv_obj_t * pkm_img = lv_img_create(t2); 
    lv_img_set_src(pkm_img, &pkm_sprite);
    lv_obj_set_size(pkm_img, 240, 240); // Redimensionado a 240
    lv_obj_center(pkm_img);
    
    // Eliminamos borde/outline para un look más limpio
    lv_obj_set_style_border_width(pkm_img, 0, 0); 
    lv_obj_set_style_bg_opa(pkm_img, 0, 0); // Fondo del sprite transparente

    // --- PANTALLA 3: STATS ---
    lv_obj_t * cont = lv_obj_create(t3);
    lv_obj_set_size(cont, 360, 360);
    lv_obj_center(cont);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);

    auto create_bar = [&](lv_obj_t ** bar, const char * label, int y, lv_color_t color) {
        lv_obj_t * l = lv_label_create(cont);
        lv_label_set_text(l, label);
        lv_obj_set_style_text_font(l, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(l, lv_color_white(), 0);
        lv_obj_align(l, LV_ALIGN_TOP_MID, 0, y);

        *bar = lv_bar_create(cont);
        lv_obj_set_size(*bar, 250, 15);
        lv_obj_align(*bar, LV_ALIGN_TOP_MID, 0, y + 25);
        lv_obj_set_style_bg_color(*bar, color, LV_PART_INDICATOR);
    };

    // He quitado el acento de "Hambre" para evitar el carácter raro
    create_bar(&hunger_bar, "Hambre:", 20, lv_color_make(255, 50, 50));
    create_bar(&happy_bar, "Amistad:", 100, lv_color_make(50, 255, 50));

    steps_label = lv_label_create(cont);
    lv_obj_set_style_text_color(steps_label, lv_color_white(), 0); // Pasos ahora en blanco
    lv_obj_align(steps_label, LV_ALIGN_BOTTOM_MID, 0, -20);

    ui_update_vpet();
}

void ui_set_time(const char * time_str) {
    lv_label_set_text(time_label, time_str);
}

void ui_update_vpet() {
    lv_label_set_text_fmt(level_label, "Nivel %d", myStatus.level);
    lv_label_set_text_fmt(batt_label, "%d%% BAT", myStatus.battery);
    lv_bar_set_value(hunger_bar, myStatus.hunger, LV_ANIM_ON);
    lv_bar_set_value(happy_bar, myStatus.happiness, LV_ANIM_ON);
    lv_label_set_text_fmt(steps_label, "Pasos: %d / %d", myStatus.steps, myStatus.step_goal);
}

void ui_update_steps() {
    ui_update_vpet();
}