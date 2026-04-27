#include "ui.h"
#include "system_data.h"

static lv_obj_t * tv;
static lv_obj_t * t_home, * t_settings, * t_widgets, * t_notifs, * t_menu;
static lv_obj_t * time_label, * level_label, * batt_label, * steps_label;
static lv_obj_t * hunger_bar, * happy_bar;
static lv_obj_t * sleep_overlay;

LV_IMG_DECLARE(pkm_sprite);
LV_IMG_DECLARE(boot_img); // Asegúrate de que en pkm_sprite.c se llame así

void ui_create_main_system();

static void start_os_timer_cb(lv_timer_t * timer) {
    ui_create_main_system();
    lv_timer_del(timer);
}

void ui_init() {
    lv_obj_t * boot_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(boot_screen, lv_color_black(), 0);

    lv_obj_t * boot_logo = lv_img_create(boot_screen);
    lv_img_set_src(boot_logo, &boot_img);
    lv_obj_center(boot_logo);

    lv_scr_load(boot_screen);
    lv_timer_create(start_os_timer_cb, 2000, NULL);
}

void ui_create_main_system() {
    lv_obj_t * main_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(main_screen, lv_color_black(), 0);

    // 1. TILEVIEW (Ocupando toda la pantalla 466x466)
    tv = lv_tileview_create(main_screen);
    lv_obj_set_size(tv, 466, 466);
    lv_obj_set_pos(tv, 0, 0); // Posición absoluta para evitar el efecto diagonal
    lv_obj_set_style_bg_opa(tv, 0, 0);
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    // 2. DEFINICIÓN DE TILES (Estructura en Cruz)
    t_notifs   = lv_tileview_add_tile(tv, 1, 0, LV_DIR_BOTTOM); // ARRIBA (Mensajes)
    t_widgets  = lv_tileview_add_tile(tv, 0, 1, LV_DIR_RIGHT);  // IZQUIERDA (Stats)
    t_home     = lv_tileview_add_tile(tv, 1, 1, LV_DIR_ALL);    // CENTRO (Home)
    t_settings = lv_tileview_add_tile(tv, 2, 1, LV_DIR_LEFT);   // DERECHA (Ajustes)
    t_menu     = lv_tileview_add_tile(tv, 1, 2, LV_DIR_TOP);    // ABAJO (Menú Apps)

    // --- PANTALLA PRINCIPAL (1,1) ---
    time_label = lv_label_create(t_home);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_align(time_label, LV_ALIGN_TOP_MID, 0, 50);

    lv_obj_t * pkm_img = lv_img_create(t_home);
    lv_img_set_src(pkm_img, &pkm_sprite);
    lv_obj_set_size(pkm_img, 240, 240);
    lv_obj_center(pkm_img);
    lv_obj_set_style_bg_opa(pkm_img, 0, 0);

    level_label = lv_label_create(t_home);
    lv_obj_set_style_text_color(level_label, lv_color_white(), 0);
    lv_obj_align(level_label, LV_ALIGN_CENTER, 0, 120);

    batt_label = lv_label_create(t_home);
    lv_obj_set_style_text_color(batt_label, lv_color_white(), 0);
    lv_obj_align(batt_label, LV_ALIGN_BOTTOM_MID, 0, -50);

    // --- PANTALLA STATS/WIDGETS (0,1) ---
    lv_obj_t * widget_cont = lv_obj_create(t_widgets);
    lv_obj_set_size(widget_cont, 340, 340);
    lv_obj_center(widget_cont);
    lv_obj_set_style_bg_opa(widget_cont, 0, 0);
    lv_obj_set_style_border_width(widget_cont, 0, 0);

    auto create_bar = [&](lv_obj_t ** bar, const char * label, int y, lv_color_t color) {
        lv_obj_t * l = lv_label_create(widget_cont);
        lv_label_set_text(l, label);
        lv_obj_set_style_text_color(l, lv_color_white(), 0);
        lv_obj_align(l, LV_ALIGN_TOP_MID, 0, y);
        *bar = lv_bar_create(widget_cont);
        lv_obj_set_size(*bar, 250, 15);
        lv_obj_align(*bar, LV_ALIGN_TOP_MID, 0, y + 25);
        lv_obj_set_style_bg_color(*bar, color, LV_PART_INDICATOR);
    };

    create_bar(&hunger_bar, "Hambre:", 20, lv_color_make(255, 50, 50));
    create_bar(&happy_bar, "Amistad:", 100, lv_color_make(50, 255, 50));

    steps_label = lv_label_create(t_widgets);
    lv_obj_set_style_text_color(steps_label, lv_color_white(), 0);
    lv_obj_align(steps_label, LV_ALIGN_BOTTOM_MID, 0, -40);

    // --- PLACEHOLDERS RESTAURADOS ---
    lv_obj_t * lbl_set = lv_label_create(t_settings);
    lv_label_set_text(lbl_set, "AJUSTES\nRAPIDOS");
    lv_obj_set_style_text_align(lbl_set, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lbl_set, lv_color_white(), 0);
    lv_obj_center(lbl_set);

    lv_obj_t * lbl_msg = lv_label_create(t_notifs);
    lv_label_set_text(lbl_msg, "MENSAJES");
    lv_obj_set_style_text_color(lbl_msg, lv_color_white(), 0);
    lv_obj_center(lbl_msg);

    lv_obj_t * lbl_men = lv_label_create(t_menu);
    lv_label_set_text(lbl_men, "MENU PRINCIPAL");
    lv_obj_set_style_text_color(lbl_men, lv_color_white(), 0);
    lv_obj_center(lbl_men);

    // --- CAPA DE SUEÑO ---
    sleep_overlay = lv_obj_create(lv_layer_sys()); 
    lv_obj_set_size(sleep_overlay, 466, 466);
    lv_obj_set_style_bg_color(sleep_overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(sleep_overlay, LV_OPA_0, 0); 
    lv_obj_set_style_border_width(sleep_overlay, 0, 0);
    lv_obj_clear_flag(sleep_overlay, LV_OBJ_FLAG_CLICKABLE);

    // CRITICAL FIX: Forzar a LVGL a calcular las posiciones antes de saltar al centro
    lv_obj_update_layout(tv);
    lv_obj_set_tile_id(tv, 1, 1, LV_ANIM_OFF); // Ahora sí saltará al centro real

    ui_update_vpet();

    // Carga con fundido y borra la pantalla de boot
    lv_scr_load_anim(main_screen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);
}

void ui_set_sleep_mode(bool sleep) {
    if (sleep) lv_obj_set_style_bg_opa(sleep_overlay, LV_OPA_COVER, 0);
    else lv_obj_set_style_bg_opa(sleep_overlay, LV_OPA_0, 0);
}

void ui_set_time(const char * time_str) { 
    if (time_label) lv_label_set_text(time_label, time_str); 
}

void ui_update_vpet() {
    if (!level_label) return;
    lv_label_set_text_fmt(level_label, "Nivel %d", myStatus.level);
    lv_label_set_text_fmt(batt_label, "%d%% BAT", myStatus.battery);
    lv_bar_set_value(hunger_bar, myStatus.hunger, LV_ANIM_ON);
    lv_bar_set_value(happy_bar, myStatus.happiness, LV_ANIM_ON);
    lv_label_set_text_fmt(steps_label, "Pasos: %d / %d", myStatus.steps, myStatus.step_goal);
}

void ui_update_steps() { ui_update_vpet(); }