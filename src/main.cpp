#include "hal.h"
#include "ui.h"
#include "system_data.h"

#ifndef ESP32
    #define millis SDL_GetTicks
#endif

SystemState myStatus;
static uint32_t last_ui_refresh = 0;
static uint32_t last_logic_refresh = 0;
bool screen_on = true;

void process_vpet_logic() {
    if (myStatus.hunger > 0) myStatus.hunger -= 1;

    // --- TEST DE EVOLUCIÓN ---
    // Nivel 1: Cosmog (0-99 pasos)
    // Nivel 2: Cosmoem (100-199 pasos) -> Evolución!
    // Nivel 3: Lunala (200+ pasos) -> Evolución!
    
    int new_level = 1;
    if (myStatus.steps >= 10) new_level = 3; // 200
    else if (myStatus.steps >= 5) new_level = 2; //100

    if (new_level != myStatus.level) {
        myStatus.level = new_level;
        // Cambiamos el nombre internamente
        if (myStatus.level == 1) snprintf(myStatus.pkm_name, 12, "Cosmog");
        if (myStatus.level == 2) snprintf(myStatus.pkm_name, 12, "Cosmoem");
        if (myStatus.level == 3) snprintf(myStatus.pkm_name, 12, "Lunala");
        
        ui_update_pkm_image(); // Ordenamos a la UI cambiar la foto
    }
}

void update_display_data() {
    int h, m;
    hal_get_time(&h, &m);

    if (h == 0 && m == 0 && myStatus.steps != 0) {
        myStatus.steps = 0;
    }

    myStatus.hour = h; myStatus.minute = m;
    myStatus.battery = hal_get_battery();
    myStatus.steps = hal_get_steps();

    char buf[10];
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

    // Lógica de Timeout (5 segundos)
    if (lv_disp_get_inactive_time(NULL) > 5000) {
        if (screen_on) {
            ui_set_sleep_mode(true); // Usamos la nueva capa negra
            screen_on = false;
        }
    } else {
        if (!screen_on) {
            ui_set_sleep_mode(false);
            screen_on = true;
        }
    }

    // Refresh de UI (solo si el sistema ya cargó y la pantalla está on)
    if (now - last_ui_refresh > 1000 && screen_on) {
        update_display_data();
        last_ui_refresh = now;
    }

    // Lógica VPet cada 5 segundos
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