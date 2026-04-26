#include "hal.h"
#include <stdlib.h>
#include <time.h>

static uint32_t simulated_steps = 0;
static int simulated_battery = 100;
static uint32_t last_sensor_update = 0;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;

#ifdef ESP32
LGFX tft;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixels((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
}
#else
SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;
SDL_Texture * texture = NULL;

void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) {
    int x, y;
    uint32_t buttons = SDL_GetMouseState(&x, &y);
    data->point.x = x;
    data->point.y = y;
    data->state = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

void monitor_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
    SDL_Rect r = { area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1 };
    SDL_UpdateTexture(texture, &r, color_p, r.w * sizeof(lv_color_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    lv_disp_flush_ready(disp_drv);
}
#endif

void hal_update_sensors() {
#ifdef ESP32
    // Aquí iría la lectura real en el futuro
#else
    uint32_t now = SDL_GetTicks();
    if (now - last_sensor_update > 3000) { // Cada 3 seg simulamos cambio
        simulated_steps += (rand() % 5);
        if (simulated_battery > 0) simulated_battery--;
        last_sensor_update = now;
    }
#endif
}

uint32_t hal_get_steps() { return simulated_steps; }
int hal_get_battery() { return simulated_battery; }

void hal_get_time(int *h, int *m) {
#ifdef ESP32
    *h = 12; *m = 0;
#else
    time_t rawtime; struct tm * timeinfo;
    time(&rawtime); timeinfo = localtime(&rawtime);
    *h = timeinfo->tm_hour; *m = timeinfo->tm_min;
#endif
}

void hal_setup() {
    lv_init();
    const int screen_w = 466;
    const int screen_h = 466;

#ifdef ESP32
    tft.init();
    tft.setBrightness(128);
    uint32_t buffer_pixels = screen_w * 40; 
    buf = (lv_color_t *)ps_malloc(buffer_pixels * sizeof(lv_color_t));
#else
    SDL_SetMainReady(); SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("RotomOS Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_w, screen_h, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);
    uint32_t buffer_pixels = screen_w * screen_h;
    buf = (lv_color_t *)malloc(buffer_pixels * sizeof(lv_color_t));
#endif

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, buffer_pixels);
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screen_w; 
    disp_drv.ver_res = screen_h;

#ifdef ESP32
    disp_drv.flush_cb = my_disp_flush;
#else
    disp_drv.flush_cb = monitor_flush;
#endif

    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

#ifndef ESP32
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read;
    lv_indev_drv_register(&indev_drv);
#endif
}

void hal_loop() {
#ifndef ESP32
    SDL_Event event;
    while(SDL_PollEvent(&event)) { if(event.type == SDL_QUIT) exit(0); }
#endif
}