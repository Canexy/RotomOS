#include "ui.h"
#include "system_data.h"
#include "app_manager.h"
#include <cstdio>

// ═══════════════════════════════════════════════════════════════════════════
// PALETA
// ═══════════════════════════════════════════════════════════════════════════
#define C_BLACK   lv_color_hex(0x000000)
#define C_BG2     lv_color_hex(0x0E0E12)
#define C_RED     lv_color_hex(0xCB3D2B)
#define C_BLUE_L  lv_color_hex(0x9BD8E9)
#define C_RED_D   lv_color_hex(0x6A302B)
#define C_BLUE_M  lv_color_hex(0x6B9EAA)
#define C_TRACK   lv_color_hex(0x151515)
#define C_TEXT    lv_color_hex(0xE8E4E0)
#define C_DIM     lv_color_hex(0x5A6A76)
#define C_ARC_LOW lv_color_hex(0x9BD8E9)
#define C_ARC_MID lv_color_hex(0xFFD600)
#define C_ARC_DONE lv_color_hex(0x32DC6E)

// ═══════════════════════════════════════════════════════════════════════════
// PUNTEROS
// ═══════════════════════════════════════════════════════════════════════════
static lv_obj_t *tv;
static lv_obj_t *t_home, *t_settings, *t_widgets, *t_notifs, *t_menu;

static lv_obj_t *time_label;
static lv_obj_t *steps_label;
static lv_obj_t *steps_arc;
static lv_obj_t *pkm_img;
static lv_obj_t *batt_home_label;
static lv_obj_t *dot_hunger;
static lv_obj_t *dot_happy;

static lv_obj_t *level_label;
static lv_obj_t *batt_label;
static lv_obj_t *hunger_bar;
static lv_obj_t *happy_bar;

static lv_obj_t *sleep_overlay;
static lv_obj_t *flashlight_obj;

static lv_obj_t *shell_screen = nullptr;

LV_IMG_DECLARE(cosmog_sprite);
LV_IMG_DECLARE(cosmoem_sprite);
LV_IMG_DECLARE(lunala_sprite);
LV_IMG_DECLARE(rotom_sprite);
LV_IMG_DECLARE(pichu_sprite);
LV_IMG_DECLARE(boot_img);

void ui_create_main_system();

// ═══════════════════════════════════════════════════════════════════════════
// ANIMACIÓN RESPIRACIÓN
// ═══════════════════════════════════════════════════════════════════════════
static void breathe_cb(void *obj, int32_t v) {
    lv_img_set_zoom((lv_obj_t *)obj, (uint16_t)v);
}

static void start_breathe_anim(lv_obj_t *img) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, img);
    lv_anim_set_exec_cb(&a, breathe_cb);
    lv_anim_set_values(&a, 248, 268);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_start(&a);
}

// ═══════════════════════════════════════════════════════════════════════════
// SPRITE
// ═══════════════════════════════════════════════════════════════════════════
void ui_update_pkm_image() {
    if (!pkm_img) return;
    lv_anim_del(pkm_img, breathe_cb);
    lv_img_set_zoom(pkm_img, 256);
    switch (myStatus.pkm_id) {
        case 172: lv_img_set_src(pkm_img, &pichu_sprite);   break;
        case 479: lv_img_set_src(pkm_img, &rotom_sprite);   break;
        case 789: lv_img_set_src(pkm_img, &cosmog_sprite);  break;
        case 790: lv_img_set_src(pkm_img, &cosmoem_sprite); break;
        case 792: lv_img_set_src(pkm_img, &lunala_sprite);  break;
        default:  lv_img_set_src(pkm_img, &cosmog_sprite);  break;
    }
    start_breathe_anim(pkm_img);
}

// ═══════════════════════════════════════════════════════════════════════════
// MODAL — OBJETIVO DE PASOS
// ═══════════════════════════════════════════════════════════════════════════
static lv_obj_t *step_modal = nullptr;
static const uint32_t step_options[] = {3000, 5000, 7500, 10000, 15000};
static int step_opt_idx = 1;

static void modal_close_cb(lv_event_t *) {
    if (step_modal) { lv_obj_del(step_modal); step_modal = nullptr; }
}

static void modal_confirm_cb(lv_event_t *e) {
    lv_obj_t *roller = (lv_obj_t *)lv_event_get_user_data(e);
    step_opt_idx = (int)lv_roller_get_selected(roller);
    myStatus.step_goal = step_options[step_opt_idx];
    modal_close_cb(nullptr);
}

static void show_step_goal_modal() {
    if (step_modal) return;

    step_modal = lv_obj_create(lv_layer_top());
    lv_obj_set_size(step_modal, 466, 466);
    lv_obj_set_style_bg_color(step_modal, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(step_modal, LV_OPA_80, 0);
    lv_obj_set_style_border_width(step_modal, 0, 0);
    lv_obj_set_style_pad_all(step_modal, 0, 0);

    lv_obj_t *card = lv_obj_create(step_modal);
    lv_obj_set_size(card, 320, 280);
    lv_obj_center(card);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x0E1520), 0);
    lv_obj_set_style_border_color(card, C_BLUE_M, 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_radius(card, 28, 0);

    lv_obj_t *title = lv_label_create(card);
    lv_label_set_text(title, "OBJETIVO DE PASOS");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14_lat, 0);
    lv_obj_set_style_text_color(title, C_BLUE_L, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 16);

    lv_obj_t *roller = lv_roller_create(card);
    lv_roller_set_options(roller, "3,000\n5,000\n7,500\n10,000\n15,000",
                          LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(roller, (uint16_t)step_opt_idx, LV_ANIM_OFF);
    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_set_width(roller, 200);
    lv_obj_align(roller, LV_ALIGN_CENTER, 0, 8);
    lv_obj_set_style_text_font(roller, &lv_font_montserrat_16_lat, 0);
    lv_obj_set_style_text_color(roller, C_TEXT, 0);
    lv_obj_set_style_bg_color(roller, lv_color_hex(0x0E1520), 0);
    lv_obj_set_style_border_width(roller, 0, 0);

    lv_obj_t *btn_ok = lv_btn_create(card);
    lv_obj_set_size(btn_ok, 110, 42);
    lv_obj_align(btn_ok, LV_ALIGN_BOTTOM_RIGHT, -12, -12);
    lv_obj_set_style_bg_color(btn_ok, C_RED, 0);
    lv_obj_set_style_radius(btn_ok, 21, 0);
    lv_obj_set_style_shadow_width(btn_ok, 0, 0);
    lv_obj_add_event_cb(btn_ok, modal_confirm_cb, LV_EVENT_CLICKED, roller);
    lv_obj_t *l_ok = lv_label_create(btn_ok);
    lv_label_set_text(l_ok, "OK");
    lv_obj_set_style_text_color(l_ok, lv_color_white(), 0);
    lv_obj_center(l_ok);

    lv_obj_t *btn_cancel = lv_btn_create(card);
    lv_obj_set_size(btn_cancel, 110, 42);
    lv_obj_align(btn_cancel, LV_ALIGN_BOTTOM_LEFT, 12, -12);
    lv_obj_set_style_bg_color(btn_cancel, lv_color_hex(0x1A2030), 0);
    lv_obj_set_style_radius(btn_cancel, 21, 0);
    lv_obj_set_style_shadow_width(btn_cancel, 0, 0);
    lv_obj_add_event_cb(btn_cancel, modal_close_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *l_c = lv_label_create(btn_cancel);
    lv_label_set_text(l_c, "CANCELAR");
    lv_obj_set_style_text_color(l_c, C_DIM, 0);
    lv_obj_set_style_text_font(l_c, &lv_font_montserrat_12_lat, 0);
    lv_obj_center(l_c);
}

// ═══════════════════════════════════════════════════════════════════════════
// EVENTOS
// ═══════════════════════════════════════════════════════════════════════════
static void fl_off_cb(lv_event_t *) { lv_obj_add_flag(flashlight_obj,   LV_OBJ_FLAG_HIDDEN); }
static void fl_on_cb (lv_event_t *) { lv_obj_clear_flag(flashlight_obj, LV_OBJ_FLAG_HIDDEN); }
static void step_goal_btn_cb(lv_event_t *) { show_step_goal_modal(); }

static void start_os_cb(lv_timer_t *t) {
    ui_create_main_system();
    lv_timer_del(t);
}

// ═══════════════════════════════════════════════════════════════════════════
// BOOT
// ═══════════════════════════════════════════════════════════════════════════
void ui_init() {
    lv_obj_t *boot = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(boot, C_BLACK, 0);
    lv_obj_t *logo = lv_img_create(boot);
    lv_img_set_src(logo, &boot_img);
    lv_obj_center(logo);
    lv_scr_load(boot);
    lv_timer_create(start_os_cb, 2500, NULL);
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════════════════════════════
static lv_obj_t* make_round_btn(lv_obj_t *parent, const char *ico,
                                 const char *lbl, lv_color_t bg,
                                 lv_event_cb_t cb = nullptr) {
    lv_obj_t *wrap = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(wrap, 0, 0);
    lv_obj_set_style_border_width(wrap, 0, 0);
    lv_obj_set_style_pad_all(wrap, 0, 0);
    lv_obj_set_size(wrap, 88, 108);
    lv_obj_clear_flag(wrap, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *circle = lv_btn_create(wrap);
    lv_obj_set_size(circle, 72, 72);
    lv_obj_align(circle, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(circle, bg, 0);
    lv_obj_set_style_shadow_width(circle, 0, 0);
    if (cb) lv_obj_add_event_cb(circle, cb, LV_EVENT_CLICKED, nullptr);

    lv_obj_t *ico_lbl = lv_label_create(circle);
    lv_label_set_text(ico_lbl, ico);
    lv_obj_set_style_text_font(ico_lbl, &lv_font_montserrat_16_lat, 0);
    lv_obj_set_style_text_color(ico_lbl, lv_color_white(), 0);
    lv_obj_center(ico_lbl);

    lv_obj_t *txt = lv_label_create(wrap);
    lv_label_set_text(txt, lbl);
    lv_obj_set_style_text_font(txt, &lv_font_montserrat_12_lat, 0);
    lv_obj_set_style_text_color(txt, C_DIM, 0);
    lv_obj_align(txt, LV_ALIGN_BOTTOM_MID, 0, 0);

    return wrap;
}

static void make_stat_card(lv_obj_t *parent, lv_obj_t **bar_out,
                            const char *title, int y, lv_color_t accent) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, 360, 80);
    lv_obj_set_pos(card, 53, y);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(card, C_BG2, 0);
    lv_obj_set_style_radius(card, 22, 0);
    lv_obj_set_style_border_color(card, accent, 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_opa(card, LV_OPA_40, 0);
    lv_obj_set_style_pad_left(card, 18, 0);
    lv_obj_set_style_pad_right(card, 18, 0);
    lv_obj_set_style_pad_top(card, 10, 0);
    lv_obj_set_style_pad_bottom(card, 10, 0);

    lv_obj_t *l = lv_label_create(card);
    lv_label_set_text(l, title);
    lv_obj_set_style_text_font(l, &lv_font_montserrat_12_lat, 0);
    lv_obj_set_style_text_color(l, accent, 0);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 0, 0);

    *bar_out = lv_bar_create(card);
    lv_obj_set_size(*bar_out, 324, 12);
    lv_obj_align(*bar_out, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_radius(*bar_out, 6, 0);
    lv_obj_set_style_radius(*bar_out, 6, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(*bar_out, C_TRACK, 0);
    lv_obj_set_style_bg_color(*bar_out, accent, LV_PART_INDICATOR);
    lv_bar_set_range(*bar_out, 0, 100);
}

// ═══════════════════════════════════════════════════════════════════════════
// SISTEMA PRINCIPAL
// ═══════════════════════════════════════════════════════════════════════════
void ui_create_overlays() {
    // Sleep overlay — creado antes del wizard y del shell
    // para que funcione desde el primer momento
    sleep_overlay = lv_obj_create(lv_layer_sys());
    lv_obj_set_size(sleep_overlay, 466, 466);
    lv_obj_set_style_bg_color(sleep_overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(sleep_overlay, LV_OPA_0, 0);
    lv_obj_clear_flag(sleep_overlay, LV_OBJ_FLAG_CLICKABLE);
}

void ui_create_main_system() {
    shell_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(shell_screen, C_BLACK, 0);

    tv = lv_tileview_create(shell_screen);
    lv_obj_set_size(tv, 466, 466);
    lv_obj_set_style_bg_color(tv, C_BLACK, 0);
    lv_obj_set_style_bg_opa(tv, LV_OPA_COVER, 0);
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    t_notifs   = lv_tileview_add_tile(tv, 1, 0, LV_DIR_BOTTOM);
    t_settings = lv_tileview_add_tile(tv, 0, 1, LV_DIR_RIGHT);
    t_home     = lv_tileview_add_tile(tv, 1, 1, LV_DIR_ALL);
    t_widgets  = lv_tileview_add_tile(tv, 2, 1, LV_DIR_LEFT);
    t_menu     = lv_tileview_add_tile(tv, 1, 2, LV_DIR_TOP);

    lv_obj_t *tiles[] = {t_notifs, t_settings, t_home, t_widgets, t_menu};
    for (auto *t : tiles) {
        lv_obj_set_style_bg_color(t, C_BLACK, 0);
        lv_obj_clear_flag(t, LV_OBJ_FLAG_SCROLLABLE);
    }

    // ── HOME ──────────────────────────────────────────────────────────────
    steps_arc = lv_arc_create(t_home);
    lv_obj_set_size(steps_arc, 450, 450);
    lv_arc_set_bg_angles(steps_arc, 0, 359);
    lv_arc_set_rotation(steps_arc, 270);
    lv_arc_set_range(steps_arc, 0, 100);
    lv_arc_set_value(steps_arc, 0);
    lv_obj_set_style_arc_width(steps_arc, 24, LV_PART_MAIN);
    lv_obj_set_style_arc_width(steps_arc, 24, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(steps_arc, C_TRACK,   LV_PART_MAIN);
    lv_obj_set_style_arc_color(steps_arc, C_ARC_LOW, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(steps_arc, true, LV_PART_INDICATOR);
    lv_obj_remove_style(steps_arc, NULL, LV_PART_KNOB);
    lv_obj_align(steps_arc, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(steps_arc, LV_OBJ_FLAG_CLICKABLE);

    time_label = lv_label_create(t_home);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48_lat, 0);
    lv_obj_set_style_text_color(time_label, C_TEXT, 0);
    lv_label_set_text(time_label, "00:00");
    lv_obj_align(time_label, LV_ALIGN_TOP_MID, 0, 68);

    steps_label = lv_label_create(t_home);
    lv_obj_set_style_text_font(steps_label, &lv_font_montserrat_14_lat, 0);
    lv_obj_set_style_text_color(steps_label, C_DIM, 0);
    lv_label_set_text(steps_label, "0 / 5,000");
    lv_obj_align(steps_label, LV_ALIGN_BOTTOM_MID, 0, -62);

    pkm_img = lv_img_create(t_home);
    lv_img_set_src(pkm_img, &cosmog_sprite);
    lv_obj_set_size(pkm_img, 200, 200);
    lv_obj_align(pkm_img, LV_ALIGN_CENTER, 0, 10);
    lv_obj_clear_flag(pkm_img, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_pivot(pkm_img, 100, 100);
    start_breathe_anim(pkm_img);

    dot_hunger = lv_obj_create(t_home);
    lv_obj_set_size(dot_hunger, 14, 14);
    lv_obj_set_style_radius(dot_hunger, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dot_hunger, C_BLUE_M, 0);
    lv_obj_set_style_border_width(dot_hunger, 0, 0);
    lv_obj_align(dot_hunger, LV_ALIGN_CENTER, -20, 120);

    dot_happy = lv_obj_create(t_home);
    lv_obj_set_size(dot_happy, 14, 14);
    lv_obj_set_style_radius(dot_happy, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dot_happy, C_BLUE_M, 0);
    lv_obj_set_style_border_width(dot_happy, 0, 0);
    lv_obj_align(dot_happy, LV_ALIGN_CENTER, 20, 120);

    batt_home_label = lv_label_create(t_home);
    lv_obj_set_style_text_font(batt_home_label, &lv_font_montserrat_12_lat, 0);
    lv_obj_set_style_text_color(batt_home_label, C_DIM, 0);
    lv_label_set_text(batt_home_label, "100%");
    lv_obj_align(batt_home_label, LV_ALIGN_BOTTOM_MID, 0, -44);

    // ── AJUSTES RAPIDOS ───────────────────────────────────────────────────
    batt_label = lv_label_create(t_settings);
    lv_obj_set_style_text_font(batt_label, &lv_font_montserrat_32_lat, 0);
    lv_obj_set_style_text_color(batt_label, C_TEXT, 0);
    lv_label_set_text(batt_label, "100%");
    lv_obj_align(batt_label, LV_ALIGN_TOP_MID, 0, 52);

    lv_obj_t *batt_sub = lv_label_create(t_settings);
    lv_label_set_text(batt_sub, "BATERIA");
    lv_obj_set_style_text_font(batt_sub, &lv_font_montserrat_12_lat, 0);
    lv_obj_set_style_text_color(batt_sub, C_DIM, 0);
    lv_obj_align(batt_sub, LV_ALIGN_TOP_MID, 0, 94);

    lv_obj_t *grid = lv_obj_create(t_settings);
    lv_obj_set_size(grid, 360, 260);
    lv_obj_align(grid, LV_ALIGN_BOTTOM_MID, 0, -24);
    lv_obj_set_style_bg_opa(grid, 0, 0);
    lv_obj_set_style_border_width(grid, 0, 0);
    lv_obj_set_style_pad_all(grid, 0, 0);
    lv_obj_clear_flag(grid, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_SPACE_EVENLY,
                                 LV_FLEX_ALIGN_CENTER,
                                 LV_FLEX_ALIGN_CENTER);

    struct { const char *ico; const char *lbl; lv_color_t c; lv_event_cb_t cb; } qs[] = {
        { "DND", "No Molestar", lv_color_hex(0x252535), nullptr  },
        { "SOL", "Brillo",      lv_color_hex(0x3A2E08), nullptr  },
        { "LUZ", "Linterna",    lv_color_hex(0x12243A), fl_on_cb },
        { "ECO", "Eco",         lv_color_hex(0x0E2418), nullptr  },
    };
    for (auto &q : qs) make_round_btn(grid, q.ico, q.lbl, q.c, q.cb);

    // ── STATS ─────────────────────────────────────────────────────────────
    level_label = lv_label_create(t_widgets);
    lv_obj_set_style_text_font(level_label, &lv_font_montserrat_48_lat, 0);
    lv_obj_set_style_text_color(level_label, C_RED, 0);
    lv_label_set_text(level_label, "1");
    lv_obj_set_pos(level_label, 198, 44);

    lv_obj_t *niv = lv_label_create(t_widgets);
    lv_label_set_text(niv, "NIVEL");
    lv_obj_set_style_text_font(niv, &lv_font_montserrat_12_lat, 0);
    lv_obj_set_style_text_color(niv, C_DIM, 0);
    lv_obj_set_pos(niv, 204, 28);

    lv_obj_t *pname = lv_label_create(t_widgets);
    lv_label_set_text(pname, myStatus.pkm_name);
    lv_obj_set_style_text_font(pname, &lv_font_montserrat_14_lat, 0);
    lv_obj_set_style_text_color(pname, C_BLUE_M, 0);
    lv_obj_set_pos(pname, 195, 100);

    make_stat_card(t_widgets, &hunger_bar, "HAMBRE",  136, C_RED_D);
    make_stat_card(t_widgets, &happy_bar,  "AMISTAD", 228, C_BLUE_M);

    // ── MENU ──────────────────────────────────────────────────────────────
    // app_id + x/y en la misma struct — una sola definición
    struct {
        const char  *ico;
        const char  *lbl;
        lv_color_t   c;
        const char  *app_id;
        int          x, y;
    } apps[] = {
        { "PKD", "PokeDex",  lv_color_hex(0x5C1A18), "pokedex",  -105, -80 },
        { "FIT", "Fitness",  lv_color_hex(0x1A3A2A), "fitness",   105, -80 },
        { "POM", "Pomodoro", lv_color_hex(0x3A2808), "pomodoro", -105,  80 },
        { "AJU", "Ajustes",  lv_color_hex(0x1A2840), nullptr,     105,  80 },
    };
    for (auto &a : apps) {
        lv_obj_t *btn = lv_btn_create(t_menu);
        lv_obj_set_size(btn, 86, 86);
        lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(btn, a.c, 0);
        lv_obj_set_style_border_color(btn, C_BLUE_M, 0);
        lv_obj_set_style_border_width(btn, 1, 0);
        lv_obj_set_style_border_opa(btn, LV_OPA_30, 0);
        lv_obj_set_style_shadow_width(btn, 0, 0);
        lv_obj_align(btn, LV_ALIGN_CENTER, a.x, a.y);
        if (a.app_id) {
            lv_obj_add_event_cb(btn, [](lv_event_t *e) {
                app_manager_launch((const char *)lv_event_get_user_data(e));
            }, LV_EVENT_CLICKED, (void *)a.app_id);
        }
        lv_obj_t *l1 = lv_label_create(btn);
        lv_label_set_text(l1, a.ico);
        lv_obj_set_style_text_font(l1, &lv_font_montserrat_12_lat, 0);
        lv_obj_set_style_text_color(l1, C_TEXT, 0);
        lv_obj_align(l1, LV_ALIGN_CENTER, 0, -8);
        lv_obj_t *l2 = lv_label_create(btn);
        lv_label_set_text(l2, a.lbl);
        lv_obj_set_style_text_font(l2, &lv_font_montserrat_12_lat, 0);
        lv_obj_set_style_text_color(l2, C_DIM, 0);
        lv_obj_align(l2, LV_ALIGN_CENTER, 0, 8);
    }

    lv_obj_t *center_btn = lv_btn_create(t_menu);
    lv_obj_set_size(center_btn, 72, 72);
    lv_obj_set_style_radius(center_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(center_btn, lv_color_hex(0x2C3C48), 0);
    lv_obj_set_style_shadow_width(center_btn, 0, 0);
    lv_obj_align(center_btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t *cl = lv_label_create(center_btn);
    lv_label_set_text(cl, "HOME");
    lv_obj_set_style_text_font(cl, &lv_font_montserrat_12_lat, 0);
    lv_obj_set_style_text_color(cl, C_DIM, 0);
    lv_obj_center(cl);

    // ── NOTIFICACIONES ────────────────────────────────────────────────────
    lv_obj_t *notif_card = lv_obj_create(t_notifs);
    lv_obj_set_size(notif_card, 340, 80);
    lv_obj_center(notif_card);
    lv_obj_set_style_bg_color(notif_card, lv_color_hex(0x1A1A2A), 0);
    lv_obj_set_style_radius(notif_card, 40, 0);
    lv_obj_set_style_border_width(notif_card, 0, 0);
    lv_obj_clear_flag(notif_card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *notif_dot = lv_obj_create(notif_card);
    lv_obj_set_size(notif_dot, 10, 10);
    lv_obj_set_style_radius(notif_dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(notif_dot, C_DIM, 0);
    lv_obj_set_style_border_width(notif_dot, 0, 0);
    lv_obj_align(notif_dot, LV_ALIGN_LEFT_MID, 18, 0);

    lv_obj_t *notif_txt = lv_label_create(notif_card);
    lv_label_set_text(notif_txt, "Sin notificaciones");
    lv_obj_set_style_text_font(notif_txt, &lv_font_montserrat_14_lat, 0);
    lv_obj_set_style_text_color(notif_txt, C_DIM, 0);
    lv_obj_align(notif_txt, LV_ALIGN_LEFT_MID, 38, 0);

    // ── CAPAS SISTEMA ─────────────────────────────────────────────────────
    flashlight_obj = lv_obj_create(lv_layer_sys());
    lv_obj_set_size(flashlight_obj, 466, 466);
    lv_obj_set_style_bg_color(flashlight_obj, lv_color_white(), 0);
    lv_obj_add_flag(flashlight_obj, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(flashlight_obj, fl_off_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_update_layout(tv);
    lv_obj_set_tile_id(tv, 1, 1, LV_ANIM_OFF);
    ui_update_pkm_image();
    ui_update_vpet();
    lv_scr_load_anim(shell_screen, LV_SCR_LOAD_ANIM_FADE_ON, 600, 0, true);
}

// ═══════════════════════════════════════════════════════════════════════════
// API PÚBLICA
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t* ui_get_shell_screen() {
    return shell_screen;
}

void ui_set_sleep_mode(bool s) {
    lv_obj_set_style_bg_opa(sleep_overlay, s ? LV_OPA_COVER : LV_OPA_0, 0);
}

void ui_set_time(const char *t) {
    if (time_label) lv_label_set_text(time_label, t);
}

void ui_update_steps() {
    if (!steps_arc || !steps_label) return;

    int pct = (int)((myStatus.steps * 100ULL) / myStatus.step_goal);
    if (pct > 100) pct = 100;
    lv_arc_set_value(steps_arc, pct);

    lv_color_t col = (pct < 50)  ? C_ARC_LOW :
                     (pct < 100) ? C_ARC_MID : C_ARC_DONE;
    lv_obj_set_style_arc_color(steps_arc, col, LV_PART_INDICATOR);

    char steps_str[24], goal_str[12];
    if (myStatus.steps >= 1000)
        snprintf(steps_str, sizeof(steps_str), "%.1fk", myStatus.steps / 1000.0f);
    else
        snprintf(steps_str, sizeof(steps_str), "%lu", myStatus.steps);

    if (myStatus.step_goal >= 1000)
        snprintf(goal_str, sizeof(goal_str), "%.0fk", myStatus.step_goal / 1000.0f);
    else
        snprintf(goal_str, sizeof(goal_str), "%lu", myStatus.step_goal);

    char buf[32];
    snprintf(buf, sizeof(buf), "%s / %s", steps_str, goal_str);
    lv_label_set_text(steps_label, buf);
}

void ui_update_vpet() {
    if (!level_label) return;

    lv_label_set_text_fmt(level_label, "%d", myStatus.level);
    if (batt_label)
        lv_label_set_text_fmt(batt_label, "%d%%", myStatus.battery);
    if (batt_home_label)
        lv_label_set_text_fmt(batt_home_label, "%d%%", myStatus.battery);
    if (hunger_bar)
        lv_bar_set_value(hunger_bar, myStatus.hunger, LV_ANIM_ON);
    if (happy_bar)
        lv_bar_set_value(happy_bar, myStatus.happiness, LV_ANIM_ON);

    if (dot_hunger) {
        lv_color_t c = (myStatus.hunger < 30) ? C_RED : C_BLUE_M;
        lv_obj_set_style_bg_color(dot_hunger, c, 0);
    }
    if (dot_happy) {
        lv_color_t c = (myStatus.happiness < 30) ? C_RED : C_BLUE_M;
        lv_obj_set_style_bg_color(dot_happy, c, 0);
    }
}