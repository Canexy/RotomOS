#include "hal.h"
#include <stdlib.h>
#include <time.h>

// ── BUFFERS LVGL ───────────────────────────────────────────────────────────
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;

// ══════════════════════════════════════════════════════════════════════════════
//  HARDWARE REAL (ESP32)
// ══════════════════════════════════════════════════════════════════════════════
#ifdef ESP32

#include <Wire.h>
#include <RTClib.h>   // RTClib – PCF8563 compatible con PCF85063

LGFX tft;

// ── Drivers I2C directos (sin librería extra) ──────────────────────────────
// Cada chip del 1.75C comparte el mismo bus I2C (SDA=15, SCL=14)

// — AXP2101 (PMIC) — dirección 0x34 ──────────────────────────────────────
static uint8_t axp_read(uint8_t reg) {
    Wire.beginTransmission(0x34);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)0x34, (uint8_t)1);
    return Wire.available() ? Wire.read() : 0;
}

static int axp_battery_percent() {
    // Registro 0xA4 del AXP2101: porcentaje de batería (0-100)
    int pct = axp_read(0xA4) & 0x7F;
    return (pct > 100) ? 100 : pct;
}

// — QMI8658 (IMU) — dirección 0x6B ────────────────────────────────────────
static void qmi_write(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(0x6B);
    Wire.write(reg); Wire.write(val);
    Wire.endTransmission();
}

static uint8_t qmi_read(uint8_t reg) {
    Wire.beginTransmission(0x6B);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)0x6B, (uint8_t)1);
    return Wire.available() ? Wire.read() : 0;
}

static void qmi_init() {
    // Reset software
    qmi_write(0x60, 0xB0);
    delay(10);
    // Habilitar acelerómetro y giroscopio
    qmi_write(0x03, 0xFF);
    // Habilitar podómetro
    qmi_write(0x14, 0x10);   // CTRL9: pedometer enable command
    qmi_write(0x15, 0x00);   // CTRL9: apply command
    delay(5);
}

static uint32_t qmi_get_steps() {
    // Registros del pedómetro: 0x1E (LSB) y 0x1F (MSB)
    uint32_t lo = qmi_read(0x1E);
    uint32_t hi = qmi_read(0x1F);
    return (hi << 8) | lo;
}

// — PCF85063 RTC ───────────────────────────────────────────────────────────
static RTC_PCF8563 rtc;   // PCF8563 es 100% compatible con PCF85063

// ── Display flush ──────────────────────────────────────────────────────────
void my_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixels((uint16_t*)&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(drv);
}

// ── Touch read ─────────────────────────────────────────────────────────────
void my_touch_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    uint16_t x, y;
    if (tft.getTouch(&x, &y)) {
        data->point.x = x;
        data->point.y = y;
        data->state   = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// ── Inicialización hardware ────────────────────────────────────────────────
void hal_setup() {
    // I2C compartido para todos los chips del 1.75C
    Wire.begin(15, 14);      // SDA=GPIO15, SCL=GPIO14
    Wire.setClock(400000);

    // RTC
    if (!rtc.begin()) {
        Serial.println("RTC no encontrado, usando hora por defecto");
    }
    // Si el RTC perdió la hora (batería agotada), ponemos una fecha base
    if (rtc.lostPower()) {
        rtc.adjust(DateTime(2025, 1, 1, 12, 0, 0));
    }

    // IMU
    qmi_init();

    // Display
    tft.init();
    tft.setBrightness(200);

    // LVGL
    lv_init();
    const int W = 466, H = 466;
    // Buffer de display en PSRAM (8MB disponibles — usar ps_malloc)
    buf = (lv_color_t*)ps_malloc(W * 40 * sizeof(lv_color_t));
    if (!buf) buf = (lv_color_t*)malloc(W * 10 * sizeof(lv_color_t)); // fallback

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, W * 40);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res  = W;
    disp_drv.ver_res  = H;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    // Activar antialiasing si la PSRAM lo permite
    disp_drv.full_refresh = 0;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touch_read;
    lv_indev_drv_register(&indev_drv);
}

void hal_get_time(int *h, int *m, int *s) {
    DateTime now = rtc.now();
    *h = now.hour();
    *m = now.minute();
    *s = now.second();
}

uint32_t hal_get_steps() {
    return qmi_get_steps();
}

int hal_get_battery() {
    return axp_battery_percent();
}

void hal_set_brightness(uint8_t val) {
    tft.setBrightness(val);
}

void hal_update_sensors() { /* sensores se leen bajo demanda */ }
void hal_loop()            { /* nada que hacer en HW real */ }

// ══════════════════════════════════════════════════════════════════════════════
//  EMULADOR PC (SDL2)
// ══════════════════════════════════════════════════════════════════════════════
#else

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture  *texture  = NULL;

static uint32_t sim_steps   = 0;
static int      sim_battery = 85;
static uint32_t last_sim_update = 0;

static void mouse_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    int x, y;
    uint32_t btns = SDL_GetMouseState(&x, &y);
    data->point.x = x;
    data->point.y = y;
    data->state   = (btns & SDL_BUTTON(SDL_BUTTON_LEFT))
                    ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

static void monitor_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p) {
    SDL_Rect r = { area->x1, area->y1,
                   area->x2 - area->x1 + 1,
                   area->y2 - area->y1 + 1 };
    SDL_UpdateTexture(texture, &r, color_p, r.w * sizeof(lv_color_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    lv_disp_flush_ready(drv);
}

void hal_setup() {
    lv_init();
    const int W = 466, H = 466;

    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
    window   = SDL_CreateWindow("RotomOS Emulator",
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                W, H, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture  = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565,
                                 SDL_TEXTUREACCESS_STREAMING, W, H);

    buf = (lv_color_t*)malloc(W * H * sizeof(lv_color_t));
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, W * H);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res  = W;
    disp_drv.ver_res  = H;
    disp_drv.flush_cb = monitor_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read;
    lv_indev_drv_register(&indev_drv);
}

void hal_get_time(int *h, int *m, int *s) {
    time_t raw; struct tm *t;
    time(&raw); t = localtime(&raw);
    *h = t->tm_hour; *m = t->tm_min; *s = t->tm_sec;
}

uint32_t hal_get_steps() { return sim_steps; }
int      hal_get_battery() { return sim_battery; }
void     hal_set_brightness(uint8_t) {}

void hal_update_sensors() {
    uint32_t now = SDL_GetTicks();
    if (now - last_sim_update > 3000) {
        sim_steps   += (rand() % 5);
        sim_battery  = (sim_battery > 5) ? sim_battery - 1 : 5;
        last_sim_update = now;
    }
}

void hal_loop() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) exit(0);
    }
}

#endif // ESP32