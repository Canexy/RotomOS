#include "ui.h"
#include "system_data.h"

// --- Punteros estáticos ---
static lv_obj_t * tv;
static lv_obj_t * t_home, * t_settings, * t_widgets, * t_notifs, * t_menu;
static lv_obj_t * time_label, * level_label, * batt_label; 
static lv_obj_t * hunger_bar, * happy_bar, * steps_arc;
static lv_obj_t * pkm_img; // El objeto de imagen ahora es estático para poder actualizarlo
static lv_obj_t * sleep_overlay, * flashlight_obj;

// --- DECLARACIÓN DE IMÁGENES ---
LV_IMG_DECLARE(cosmog_sprite);  // Renombrado de pkm_sprite
LV_IMG_DECLARE(cosmoem_sprite); 
LV_IMG_DECLARE(lunala_sprite);  
LV_IMG_DECLARE(boot_img);

void ui_create_main_system();

// --- LÓGICA DE EVOLUCIÓN (Cambio de imagen) ---
void ui_update_pkm_image() {
    if (!pkm_img) return;

    // Cambiamos la fuente de la imagen según el nivel definido en main.cpp
    if (myStatus.level == 1) lv_img_set_src(pkm_img, &cosmog_sprite);
    else if (myStatus.level == 2) lv_img_set_src(pkm_img, &cosmoem_sprite);
    else if (myStatus.level == 3) lv_img_set_src(pkm_img, &lunala_sprite);
}

// --- EVENTOS ---
static void flashlight_off_event_cb(lv_event_t * e) { lv_obj_add_flag(flashlight_obj, LV_OBJ_FLAG_HIDDEN); }
static void flashlight_on_event_cb(lv_event_t * e) { lv_obj_clear_flag(flashlight_obj, LV_OBJ_FLAG_HIDDEN); }

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

    tv = lv_tileview_create(main_screen);
    lv_obj_set_size(tv, 466, 466);
    lv_obj_set_style_bg_opa(tv, 0, 0);
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    // Configuración de Tiles (Cruz 2D)
    t_notifs   = lv_tileview_add_tile(tv, 1, 0, LV_DIR_BOTTOM); 
    t_settings = lv_tileview_add_tile(tv, 0, 1, LV_DIR_RIGHT);  
    t_home     = lv_tileview_add_tile(tv, 1, 1, LV_DIR_ALL);    
    t_widgets  = lv_tileview_add_tile(tv, 2, 1, LV_DIR_LEFT);   
    t_menu     = lv_tileview_add_tile(tv, 1, 2, LV_DIR_TOP);    

    // --- PANTALLA PRINCIPAL (CENTRO: 1,1) ---
    
    // 1. Reloj (Arriba)
    time_label = lv_label_create(t_home);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_align(time_label, LV_ALIGN_TOP_MID, 0, 65);

    // 2. Anillo de Pasos (Independiente)
    steps_arc = lv_arc_create(t_home);
    lv_obj_set_size(steps_arc, 320, 320); // 300 y 300 originalmente; ajuste estético personal
    lv_arc_set_rotation(steps_arc, 135);
    lv_arc_set_bg_angles(steps_arc, 0, 270);
    lv_arc_set_value(steps_arc, 0);
    lv_obj_set_style_arc_width(steps_arc, 15, LV_PART_MAIN);
    lv_obj_set_style_arc_width(steps_arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(steps_arc, lv_color_make(40, 40, 40), LV_PART_MAIN);
    lv_obj_set_style_arc_color(steps_arc, lv_color_make(255, 200, 0), LV_PART_INDICATOR);
    lv_obj_remove_style(steps_arc, NULL, LV_PART_KNOB);
    lv_obj_align(steps_arc, LV_ALIGN_CENTER, 0, 50); // 45 originalmente; ajuste estético personal
    lv_obj_clear_flag(steps_arc, LV_OBJ_FLAG_CLICKABLE);

    // 3. Sprite del Pokémon (Independiente)
    pkm_img = lv_img_create(t_home);
    lv_img_set_src(pkm_img, &cosmog_sprite); // Imagen inicial
    lv_obj_set_size(pkm_img, 240, 240);
    
    // Al usar lv_obj_align en lugar de lv_obj_align_to, el sprite es independiente del arco
    lv_obj_align(pkm_img, LV_ALIGN_CENTER, 0, 45); // Altura ajustable del sprite
    lv_obj_clear_flag(pkm_img, LV_OBJ_FLAG_CLICKABLE);

    // --- PANTALLA AJUSTES (IZQUIERDA: 0,1) ---
    lv_obj_t * set_cont = lv_obj_create(t_settings);
    lv_obj_set_size(set_cont, 380, 380);
    lv_obj_center(set_cont);
    lv_obj_set_style_bg_opa(set_cont, 0, 0);
    lv_obj_set_style_border_width(set_cont, 0, 0);

    batt_label = lv_label_create(set_cont);
    lv_obj_set_style_text_color(batt_label, lv_color_white(), 0);
    lv_obj_align(batt_label, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * grid = lv_obj_create(set_cont);
    lv_obj_set_size(grid, 340, 280);
    lv_obj_align(grid, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(grid, 0, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(grid, 20, 0);

    auto add_q_btn = [&](const char * t, lv_color_t c) {
        lv_obj_t * b = lv_btn_create(grid);
        lv_obj_set_size(b, 110, 110);
        lv_obj_set_style_radius(b, 25, 0);
        lv_obj_set_style_bg_color(b, c, 0);
        lv_obj_t * l = lv_label_create(b);
        lv_label_set_text(l, t);
        lv_obj_center(l);
        return b;
    };

    add_q_btn("DND", lv_color_make(60, 60, 60));
    add_q_btn("BRI", lv_color_make(180, 180, 0));
    lv_obj_t * lb = add_q_btn("LINTER", lv_color_make(0, 120, 200));
    lv_obj_add_event_cb(lb, flashlight_on_event_cb, LV_EVENT_CLICKED, NULL);
    add_q_btn("ECO", lv_color_make(0, 150, 0));

    // --- PANTALLA STATS (DERECHA: 2,1) ---
    lv_obj_t * widget_cont = lv_obj_create(t_widgets);
    lv_obj_set_size(widget_cont, 360, 360);
    lv_obj_center(widget_cont);
    lv_obj_set_style_bg_opa(widget_cont, 0, 0);
    lv_obj_set_style_border_width(widget_cont, 0, 0);

    level_label = lv_label_create(widget_cont); 
    lv_obj_set_style_text_font(level_label, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(level_label, lv_color_white(), 0);
    lv_obj_align(level_label, LV_ALIGN_TOP_MID, 0, 20);

    auto create_bar = [&](lv_obj_t ** bar, const char * label, int y, lv_color_t color) {
        lv_obj_t * l = lv_label_create(widget_cont);
        lv_label_set_text(l, label);
        lv_obj_set_style_text_color(l, lv_color_white(), 0);
        lv_obj_align(l, LV_ALIGN_TOP_MID, 0, y);
        *bar = lv_bar_create(widget_cont);
        lv_obj_set_size(*bar, 250, 20);
        lv_obj_align(*bar, LV_ALIGN_TOP_MID, 0, y + 25);
        lv_obj_set_style_bg_color(*bar, color, LV_PART_INDICATOR);
    };

    create_bar(&hunger_bar, "Hambre:", 90, lv_color_make(255, 50, 50));
    create_bar(&happy_bar, "Amistad:", 170, lv_color_make(50, 255, 50));

    // --- PANTALLA MENÚ (ABAJO: 1,2) ---
    lv_obj_t * menu_cont = lv_obj_create(t_menu);
    lv_obj_set_size(menu_cont, 400, 350);
    lv_obj_center(menu_cont);
    lv_obj_set_flex_flow(menu_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(menu_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(menu_cont, 0, 0);
    lv_obj_set_style_border_width(menu_cont, 0, 0);
    lv_obj_set_style_pad_gap(menu_cont, 20, 0);

    auto add_app = [&](const char * n, lv_color_t c) {
        lv_obj_t * b = lv_btn_create(menu_cont);
        lv_obj_set_size(b, 280, 70);
        lv_obj_set_style_radius(b, 35, 0);
        lv_obj_set_style_bg_color(b, c, 0);
        lv_obj_t * l = lv_label_create(b);
        lv_label_set_text(l, n);
        lv_obj_center(l);
    };
    add_app("POKEDEX", lv_color_make(180, 0, 0));
    add_app("FITNESS", lv_color_make(0, 120, 0));
    add_app("POMODORO", lv_color_make(220, 80, 0));

    // --- PANTALLA MENSAJES (ARRIBA: 1,0) ---
    lv_obj_t * lbl_msg = lv_label_create(t_notifs);
    lv_label_set_text(lbl_msg, "MENSAJES");
    lv_obj_set_style_text_color(lbl_msg, lv_color_white(), 0);
    lv_obj_center(lbl_msg);

    // --- CAPAS SISTEMA ---
    flashlight_obj = lv_obj_create(lv_layer_sys());
    lv_obj_set_size(flashlight_obj, 466, 466);
    lv_obj_set_style_bg_color(flashlight_obj, lv_color_white(), 0);
    lv_obj_add_flag(flashlight_obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(flashlight_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(flashlight_obj, flashlight_off_event_cb, LV_EVENT_CLICKED, NULL);

    sleep_overlay = lv_obj_create(lv_layer_sys()); 
    lv_obj_set_size(sleep_overlay, 466, 466);
    lv_obj_set_style_bg_color(sleep_overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(sleep_overlay, LV_OPA_0, 0); 
    lv_obj_clear_flag(sleep_overlay, LV_OBJ_FLAG_CLICKABLE);

    // Ajustes finales
    lv_obj_update_layout(tv);
    lv_obj_set_tile_id(tv, 1, 1, LV_ANIM_OFF);
    ui_update_vpet();
    ui_update_pkm_image(); // Sincroniza imagen inicial
    lv_scr_load_anim(main_screen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);
}

void ui_set_sleep_mode(bool s) {
    lv_obj_set_style_bg_opa(sleep_overlay, s ? LV_OPA_COVER : LV_OPA_0, 0);
}

void ui_set_time(const char * t) { if (time_label) lv_label_set_text(time_label, t); }

void ui_update_vpet() {
    if (!level_label) return;
    lv_label_set_text_fmt(level_label, "Nivel %d", myStatus.level);
    lv_label_set_text_fmt(batt_label, "BATERIA: %d%%", myStatus.battery);
    lv_bar_set_value(hunger_bar, myStatus.hunger, LV_ANIM_ON);
    lv_bar_set_value(happy_bar, myStatus.happiness, LV_ANIM_ON);

    // Actualizar anillo de pasos dinámico
    int perc = (myStatus.steps * 100) / myStatus.step_goal;
    lv_arc_set_value(steps_arc, perc > 100 ? 100 : perc);
}

void ui_update_steps() { ui_update_vpet(); }