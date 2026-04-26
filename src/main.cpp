#include "hal.h"
#include "ui.h"
#include "system_data.h"

#ifndef ESP32
    #define millis SDL_GetTicks
#endif

SystemState myStatus; 
uint32_t last_ui_tick = 0;
uint32_t last_vpet_tick = 0;

void setup() {
    hal_setup();
    ui_init();
}

void loop() {
    hal_loop();
    hal_update_sensors();
    
    uint32_t now = millis();

    // Actualización rápida (UI y Tiempo)
    if (now - last_ui_tick > 1000) {
        myStatus.steps = hal_get_steps();
        myStatus.battery = hal_get_battery();
        
        int h, m;
        hal_get_time(&h, &m);
        myStatus.hour = h; myStatus.minute = m;

        char buf[10];
        snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
        ui_set_time(buf);
        ui_update_steps();
        ui_update_vpet();
        
        last_ui_tick = now;
    }

    // Lógica de vida (Hambre cada 10 segundos)
    if (now - last_vpet_tick > 10000) {
        if(myStatus.hunger > 0) myStatus.hunger -= 2;
        if(myStatus.hunger < 20 && myStatus.happiness > 0) myStatus.happiness--;
        
        last_vpet_tick = now;
    }

    lv_timer_handler(); 

#ifndef ESP32
    SDL_Delay(5);
#endif
}

#ifndef ESP32
int main(int argc, char** argv) { setup(); while(1) loop(); return 0; }
#endif