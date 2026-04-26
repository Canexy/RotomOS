#include "hal.h"
#include "ui.h"
#include "system_data.h"

#ifndef ESP32
    #define millis SDL_GetTicks
#endif

SystemState myStatus;
static uint32_t last_ui_refresh = 0;
static uint32_t last_logic_refresh = 0;

void process_vpet_logic() {
    // Simulación de degradación de stats
    if (myStatus.hunger > 0) myStatus.hunger -= 1;
    if (myStatus.steps % 50 == 0 && myStatus.steps > 0) {
        myStatus.exp += 10;
        if (myStatus.exp >= 100) { myStatus.level++; myStatus.exp = 0; }
    }
}

void update_display_data() {
    int h, m;
    hal_get_time(&h, &m);
    myStatus.hour = h; myStatus.minute = m;
    myStatus.battery = hal_get_battery();
    myStatus.steps = hal_get_steps();

    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
    ui_set_time(buf);
    ui_update_steps();
    ui_update_vpet();
}

void setup() {
    hal_setup();
    ui_init();
}

void loop() {
    hal_loop();
    hal_update_sensors();

    uint32_t now = millis();

    // Refresh de UI (1 segundo)
    if (now - last_ui_refresh > 1000) {
        update_display_data();
        last_ui_refresh = now;
    }

    // Lógica VPet (5 segundos para pruebas)
    if (now - last_logic_refresh > 5000) {
        process_vpet_logic();
        last_logic_refresh = now;
    }

    lv_timer_handler();
#ifndef ESP32
    SDL_Delay(5);
#endif
}

#ifndef ESP32
int main(int argc, char** argv) { setup(); while(1) loop(); return 0; }
#endif