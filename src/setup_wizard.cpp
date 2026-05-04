#include "setup_wizard.h"
#include "persistence.h"
#include "system_data.h"
#include "ui.h"
#include <string.h>
#include <cstdio>

// ── Fonts ─────────────────────────────────────────────────────────────────
LV_FONT_DECLARE(lv_font_montserrat_12_lat);
LV_FONT_DECLARE(lv_font_montserrat_14_lat);
LV_FONT_DECLARE(lv_font_montserrat_16_lat);
LV_FONT_DECLARE(lv_font_montserrat_32_lat);
LV_FONT_DECLARE(lv_font_montserrat_48_lat);

// ── Paleta ────────────────────────────────────────────────────────────────
#define C_BLACK  lv_color_hex(0x000000)
#define C_RED    lv_color_hex(0xCB3D2B)
#define C_BLUE_L lv_color_hex(0x9BD8E9)
#define C_BLUE_M lv_color_hex(0x6B9EAA)
#define C_TEXT   lv_color_hex(0xE8E4E0)
#define C_DIM    lv_color_hex(0x5A6A76)
#define C_BG2    lv_color_hex(0x0E0E12)

// ── Sprites de starters ───────────────────────────────────────────────────
LV_IMG_DECLARE(cosmog_sprite);
LV_IMG_DECLARE(rotom_sprite);
LV_IMG_DECLARE(pichu_sprite);

// ── Starters disponibles ──────────────────────────────────────────────────
struct StarterInfo {
    uint16_t         id;
    const char      *name;
    const char      *desc;
    lv_color_t       color;
    const lv_img_dsc_t *sprite;
};

static const StarterInfo starters[] = {
    { 789, "Cosmog",  "Nebuloso y misterioso",    lv_color_hex(0x4A6AD8), &cosmog_sprite },
    { 479, "Rotom",   "Espiritu del dispositivo", lv_color_hex(0x58C4DC), &rotom_sprite  },
    { 172, "Pichu",   "Chispeante y travieso",    lv_color_hex(0xF0D040), &pichu_sprite  },
};
static const int STARTER_COUNT = 3;

// ── Estado interno del wizard ─────────────────────────────────────────────
static int     selected_starter = 0;
static lv_obj_t *scr_welcome  = nullptr;
static lv_obj_t *scr_starter  = nullptr;
static lv_obj_t *starter_cards[3] = {};

// ── Prototipos internos ───────────────────────────────────────────────────
static void show_welcome();
static void show_starter_select();
static void show_time_setup();
static void finish_wizard();
static void highlight_selected_card();

// ═══════════════════════════════════════════════════════════════════════════
// PANTALLA 1 — BIENVENIDA
// ═══════════════════════════════════════════════════════════════════════════
static void welcome_next_cb(lv_event_t *) {
    show_starter_select();
}

static void show_welcome() {
    scr_welcome = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_welcome, C_BLACK, 0);
    lv_obj_clear_flag(scr_welcome, LV_OBJ_FLAG_SCROLLABLE);

    // Logo / título
    lv_obj_t *title = lv_label_create(scr_welcome);
    lv_label_set_text(title, "RotomOS");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_48_lat, 0);
    lv_obj_set_style_text_color(title, C_BLUE_L, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -80);

    lv_obj_t *sub = lv_label_create(scr_welcome);
    lv_label_set_text(sub, "Bienvenido, entrenador");
    lv_obj_set_style_text_font(sub, &lv_font_montserrat_16_lat, 0);
    lv_obj_set_style_text_color(sub, C_TEXT, 0);
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t *hint = lv_label_create(scr_welcome);
    lv_label_set_text(hint, "Vamos a configurar tu dispositivo");
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_12_lat, 0);
    lv_obj_set_style_text_color(hint, C_DIM, 0);
    lv_obj_align(hint, LV_ALIGN_CENTER, 0, 20);

    // Botón continuar
    lv_obj_t *btn = lv_btn_create(scr_welcome);
    lv_obj_set_size(btn, 200, 52);
    lv_obj_set_style_radius(btn, 26, 0);
    lv_obj_set_style_bg_color(btn, C_RED, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_obj_add_event_cb(btn, welcome_next_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *bl = lv_label_create(btn);
    lv_label_set_text(bl, "Comenzar");
    lv_obj_set_style_text_font(bl, &lv_font_montserrat_16_lat, 0);
    lv_obj_set_style_text_color(bl, C_TEXT, 0);
    lv_obj_center(bl);

    lv_scr_load(scr_welcome);
}

// ═══════════════════════════════════════════════════════════════════════════
// PANTALLA 2 — ELECCION DE COMPAÑERO
// ═══════════════════════════════════════════════════════════════════════════
static void card_click_cb(lv_event_t *e) {
    selected_starter = (int)(intptr_t)lv_event_get_user_data(e);
    highlight_selected_card();
}

static void starter_next_cb(lv_event_t *) {
    finish_wizard(); // directo a finalizar, sin paso de hora
}

static void highlight_selected_card() {
    for (int i = 0; i < STARTER_COUNT; i++) {
        if (!starter_cards[i]) continue;
        lv_color_t border = (i == selected_starter)
                            ? starters[i].color
                            : lv_color_hex(0x1A1A2A);
        int bw = (i == selected_starter) ? 2 : 1;
        lv_obj_set_style_border_color(starter_cards[i], border, 0);
        lv_obj_set_style_border_width(starter_cards[i], bw, 0);
    }
}

static void show_starter_select() {
    scr_starter = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_starter, C_BLACK, 0);
    lv_obj_clear_flag(scr_starter, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(scr_starter);
    lv_label_set_text(title, "Elige tu compañero");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16_lat, 0);
    lv_obj_set_style_text_color(title, C_TEXT, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 36);

    // Tarjetas de starters — disposición horizontal centrada
    for (int i = 0; i < STARTER_COUNT; i++) {
        starter_cards[i] = lv_obj_create(scr_starter);
        lv_obj_set_size(starter_cards[i], 120, 160);
        lv_obj_set_pos(starter_cards[i], 28 + i * 138, 80);
        lv_obj_set_style_bg_color(starter_cards[i], C_BG2, 0);
        lv_obj_set_style_radius(starter_cards[i], 18, 0);
        lv_obj_set_style_border_color(starter_cards[i], lv_color_hex(0x1A1A2A), 0);
        lv_obj_set_style_border_width(starter_cards[i], 1, 0);
        lv_obj_set_style_pad_all(starter_cards[i], 0, 0);
        lv_obj_clear_flag(starter_cards[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(starter_cards[i], LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(starter_cards[i], card_click_cb,
                            LV_EVENT_CLICKED, (void *)(intptr_t)i);

        // Sprite del Pokémon (zoom desde 240px original a ~80px)
        lv_obj_t *img = lv_img_create(starter_cards[i]);
        lv_img_set_src(img, starters[i].sprite);
        lv_img_set_zoom(img, 85);  // 256*80/240 ≈ 85
        lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 8);
        lv_obj_clear_flag(img, LV_OBJ_FLAG_CLICKABLE);

        // Nombre
        lv_obj_t *name = lv_label_create(starter_cards[i]);
        lv_label_set_text(name, starters[i].name);
        lv_obj_set_style_text_font(name, &lv_font_montserrat_14_lat, 0);
        lv_obj_set_style_text_color(name, C_TEXT, 0);
        lv_obj_align(name, LV_ALIGN_TOP_MID, 0, 98);

        // Descripción
        lv_obj_t *desc = lv_label_create(starter_cards[i]);
        lv_label_set_text(desc, starters[i].desc);
        lv_obj_set_style_text_font(desc, &lv_font_montserrat_12_lat, 0);
        lv_obj_set_style_text_color(desc, C_DIM, 0);
        lv_label_set_long_mode(desc, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(desc, 100);
        lv_obj_align(desc, LV_ALIGN_TOP_MID, 0, 122);
    }

    highlight_selected_card();

    // Botón confirmar
    lv_obj_t *btn = lv_btn_create(scr_starter);
    lv_obj_set_size(btn, 200, 52);
    lv_obj_set_style_radius(btn, 26, 0);
    lv_obj_set_style_bg_color(btn, C_RED, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -28);
    lv_obj_add_event_cb(btn, starter_next_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *bl = lv_label_create(btn);
    lv_label_set_text(bl, "Elegir");
    lv_obj_set_style_text_font(bl, &lv_font_montserrat_16_lat, 0);
    lv_obj_set_style_text_color(bl, C_TEXT, 0);
    lv_obj_center(bl);

    lv_scr_load_anim(scr_starter, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, true);
}

// Hora: gestionada por BLE sync (Fase 2) o ajuste manual en Ajustes

// ═══════════════════════════════════════════════════════════════════════════
// FINALIZAR WIZARD
// ═══════════════════════════════════════════════════════════════════════════
static void finish_wizard() {
    const StarterInfo &s = starters[selected_starter];
    myStatus.pkm_id = s.id;
    strncpy(myStatus.pkm_name, s.name, 16);
    myStatus.level      = 1;
    myStatus.exp        = 0;
    myStatus.hunger     = 100;
    myStatus.happiness  = 100;
    myStatus.first_boot = false;

    persistence_save();
    ui_init();
}

// ═══════════════════════════════════════════════════════════════════════════
// ENTRADA PÚBLICA
// ═══════════════════════════════════════════════════════════════════════════
void setup_wizard_start() {
    selected_starter = 0;
    show_welcome();
}