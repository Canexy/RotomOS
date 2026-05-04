#include "app_manager.h"
#include "system_data.h"
#include <cstdio>

#define C_BLACK   lv_color_hex(0x000000)
#define C_BG2     lv_color_hex(0x0E0E12)
#define C_RED     lv_color_hex(0xCB3D2B)
#define C_BLUE_L  lv_color_hex(0x9BD8E9)
#define C_BLUE_M  lv_color_hex(0x6B9EAA)
#define C_TRACK   lv_color_hex(0x151515)
#define C_TEXT    lv_color_hex(0xE8E4E0)
#define C_DIM     lv_color_hex(0x5A6A76)
#define C_ARC_LOW lv_color_hex(0x9BD8E9)
#define C_ARC_MID lv_color_hex(0xFFD600)
#define C_ARC_DONE lv_color_hex(0x32DC6E)

LV_FONT_DECLARE(lv_font_montserrat_12_lat);
LV_FONT_DECLARE(lv_font_montserrat_14_lat);
LV_FONT_DECLARE(lv_font_montserrat_48_lat);

// Forward declaration
void app_fitness_tick();

// ── Estado interno ────────────────────────────────────────────────────────
static lv_obj_t *scr       = nullptr;
static lv_obj_t *arc       = nullptr;
static lv_obj_t *steps_lbl = nullptr;
static lv_obj_t *goal_lbl  = nullptr;
static lv_obj_t *pct_lbl   = nullptr;
static lv_obj_t *day_dots[7] = {};

static const char *DAY_NAMES[] = {"L","M","X","J","V","S","D"};

static void back_cb(lv_event_t *) { app_manager_exit(); }

// ── Launch ────────────────────────────────────────────────────────────────
void app_fitness_launch() {
    scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, C_BLACK, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    // Arco grande de pasos
    arc = lv_arc_create(scr);
    lv_obj_set_size(arc, 380, 380);
    lv_arc_set_bg_angles(arc, 140, 400);
    lv_arc_set_rotation(arc, 0);
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_value(arc, 0);
    lv_obj_set_style_arc_width(arc, 22, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 22, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, C_TRACK,   LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, C_ARC_LOW, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_INDICATOR);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_align(arc, LV_ALIGN_CENTER, 0, -10);
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);

    // Número de pasos
    steps_lbl = lv_label_create(scr);
    lv_obj_set_style_text_font(steps_lbl, &lv_font_montserrat_48_lat, 0);
    lv_obj_set_style_text_color(steps_lbl, C_TEXT, 0);
    lv_label_set_text(steps_lbl, "0");
    lv_obj_align(steps_lbl, LV_ALIGN_CENTER, 0, -30);

    lv_obj_t *unit = lv_label_create(scr);
    lv_obj_set_style_text_font(unit, &lv_font_montserrat_12_lat, 0);
    lv_obj_set_style_text_color(unit, C_DIM, 0);
    lv_label_set_text(unit, "pasos hoy");
    lv_obj_align(unit, LV_ALIGN_CENTER, 0, 28);

    goal_lbl = lv_label_create(scr);
    lv_obj_set_style_text_font(goal_lbl, &lv_font_montserrat_14_lat, 0);
    lv_obj_set_style_text_color(goal_lbl, C_BLUE_M, 0);
    lv_label_set_text(goal_lbl, "/ 5,000");
    lv_obj_align(goal_lbl, LV_ALIGN_CENTER, 0, 52);

    pct_lbl = lv_label_create(scr);
    lv_obj_set_style_text_font(pct_lbl, &lv_font_montserrat_12_lat, 0);
    lv_obj_set_style_text_color(pct_lbl, C_DIM, 0);
    lv_label_set_text(pct_lbl, "0%");
    lv_obj_align(pct_lbl, LV_ALIGN_CENTER, 0, 78);

    // Días de la semana
    int today = 0; // TODO: hal_get_weekday() en Fase 2
    for (int i = 0; i < 7; i++) {
        lv_obj_t *col = lv_obj_create(scr);
        lv_obj_set_size(col, 36, 52);
        lv_obj_set_pos(col, 38 + i * 56, 360);
        lv_obj_set_style_bg_opa(col, 0, 0);
        lv_obj_set_style_border_width(col, 0, 0);
        lv_obj_set_style_pad_all(col, 0, 0);
        lv_obj_clear_flag(col, LV_OBJ_FLAG_SCROLLABLE);

        day_dots[i] = lv_obj_create(col);
        lv_obj_set_size(day_dots[i], 16, 16);
        lv_obj_set_style_radius(day_dots[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(day_dots[i], 0, 0);
        lv_obj_set_style_bg_color(day_dots[i], (i == today) ? C_RED : C_TRACK, 0);
        lv_obj_align(day_dots[i], LV_ALIGN_TOP_MID, 0, 0);

        lv_obj_t *dlbl = lv_label_create(col);
        lv_label_set_text(dlbl, DAY_NAMES[i]);
        lv_obj_set_style_text_font(dlbl, &lv_font_montserrat_12_lat, 0);
        lv_obj_set_style_text_color(dlbl, (i == today) ? C_TEXT : C_DIM, 0);
        lv_obj_align(dlbl, LV_ALIGN_BOTTOM_MID, 0, 0);
    }

    // Botón atrás
    lv_obj_t *back = lv_btn_create(scr);
    lv_obj_set_size(back, 52, 52);
    lv_obj_set_style_radius(back, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(back, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_shadow_width(back, 0, 0);
    lv_obj_align(back, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_add_event_cb(back, back_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *bl = lv_label_create(back);
    lv_label_set_text(bl, "<");
    lv_obj_set_style_text_font(bl, &lv_font_montserrat_14_lat, 0);
    lv_obj_set_style_text_color(bl, C_TEXT, 0);
    lv_obj_center(bl);

    lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    app_fitness_tick();
}

// ── Destroy ───────────────────────────────────────────────────────────────
void app_fitness_destroy() {
    // Solo nulamos punteros.
    // lv_scr_load_anim(del_prev=true) borra el lv_obj al terminar la animación.
    scr = nullptr; arc = nullptr; steps_lbl = nullptr;
    goal_lbl = nullptr; pct_lbl = nullptr;
    for (int i = 0; i < 7; i++) day_dots[i] = nullptr;
}

// ── Tick ─────────────────────────────────────────────────────────────────
void app_fitness_tick() {
    if (!scr) return;

    char buf[16];
    if (myStatus.steps >= 1000)
        snprintf(buf, sizeof(buf), "%.1fk", myStatus.steps / 1000.0f);
    else
        snprintf(buf, sizeof(buf), "%lu", myStatus.steps);
    lv_label_set_text(steps_lbl, buf);

    char gbuf[16];
    if (myStatus.step_goal >= 1000)
        snprintf(gbuf, sizeof(gbuf), "/ %.0fk", myStatus.step_goal / 1000.0f);
    else
        snprintf(gbuf, sizeof(gbuf), "/ %lu", myStatus.step_goal);
    lv_label_set_text(goal_lbl, gbuf);

    int pct = (int)((myStatus.steps * 100ULL) / myStatus.step_goal);
    if (pct > 100) pct = 100;
    lv_arc_set_value(arc, pct);

    lv_color_t col = (pct < 50)  ? C_ARC_LOW :
                     (pct < 100) ? C_ARC_MID : C_ARC_DONE;
    lv_obj_set_style_arc_color(arc, col, LV_PART_INDICATOR);

    char pbuf[8];
    snprintf(pbuf, sizeof(pbuf), "%d%%", pct);
    lv_label_set_text(pct_lbl, pbuf);
}