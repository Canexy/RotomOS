#include "hal.h"
#include "ui.h"
#include "system_data.h"

#ifndef ESP32
    #define millis SDL_GetTicks
#endif

SystemState myStatus;

static uint32_t last_ui_refresh    = 0;
static uint32_t last_logic_refresh = 0;
bool screen_on = true;

// ── Tabla de evoluciones (extensible) ─────────────────────────────────────
// Formato: { id_actual, nombre, nivel_evo, id_siguiente }
// Nivel basado en pasos reales (5000 pasos ~ nivel vPet)
struct EvoEntry { uint16_t id; const char *name; uint32_t step_trigger; uint16_t evo_id; };

static const EvoEntry evo_table[] = {
    { 789, "Cosmog",  5000, 790 },   // Cosmog  → Cosmoem a 5.000 pasos
    { 790, "Cosmoem", 15000, 792 },  // Cosmoem → Lunala  a 15.000 pasos
    { 792, "Lunala",  0,    0   },   // Lunala  → no evoluciona
};

static const EvoEntry* find_evo(uint16_t id) {
    for (auto &e : evo_table) if (e.id == id) return &e;
    return nullptr;
}

// ── Lógica vPet ───────────────────────────────────────────────────────────
void process_vpet_logic() {
    // ── Hambre: baja con el tiempo ─────────────────────────────────────
    if (myStatus.hunger > 0) myStatus.hunger -= 1;

    // ── Felicidad: sistema de balance ──────────────────────────────────
    // Pérdida base lenta
    int delta = -1;

    // Penalización si tiene mucha hambre
    if (myStatus.hunger < 30) delta -= 2;

    // Ganancia por pasos nuevos desde el último ciclo
    static uint32_t prev_steps = 0;
    if (myStatus.steps > prev_steps) {
        uint32_t new_steps = myStatus.steps - prev_steps;
        delta += (int)(new_steps / 50); // +1 felicidad por cada 50 pasos nuevos
    }
    prev_steps = myStatus.steps;

    myStatus.happiness = (int)myStatus.happiness + delta;
    if (myStatus.happiness < 0)   myStatus.happiness = 0;
    if (myStatus.happiness > 100) myStatus.happiness = 100;

    // ── Evolución (sin cambios) ─────────────────────────────────────────
    const EvoEntry *e = find_evo(myStatus.pkm_id);
    if (e && e->step_trigger > 0 && myStatus.steps >= e->step_trigger) {
        myStatus.pkm_id = e->evo_id;
        const EvoEntry *next = find_evo(e->evo_id);
        if (next) snprintf(myStatus.pkm_name, 16, "%s", next->name);
        myStatus.level++;
        ui_update_pkm_image();
    }
}

// ── Actualización de datos de pantalla ────────────────────────────────────
void update_display_data() {
    int h, m, s;
    hal_get_time(&h, &m, &s);
    myStatus.hour   = h;
    myStatus.minute = m;
    myStatus.second = s;
    myStatus.battery = hal_get_battery();
    myStatus.steps   = hal_get_steps();

    // Reset diario de pasos a medianoche (por día del mes)
    // NOTA: en HW real el RTC da el día. En emulador usamos hora 00:00
    if (h == 0 && m == 0) {
        myStatus.steps = 0;
    }

    char buf[10];
    snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
    ui_set_time(buf);
    ui_update_steps();    // solo actualiza el arco
    ui_update_vpet();     // solo actualiza stats
}

// ── Setup / Loop ───────────────────────────────────────────────────────────
void setup() {
#ifdef ESP32
    Serial.begin(115200);
#endif
    hal_setup();
    ui_init();
}

void loop() {
    hal_loop();
    hal_update_sensors();

    uint32_t now = millis();

    // Timeout pantalla (5 s sin tocar → sleep)
    if (lv_disp_get_inactive_time(NULL) > 5000) {
        if (screen_on) {
            ui_set_sleep_mode(true);
            hal_set_brightness(0);
            screen_on = false;
        }
    } else {
        if (!screen_on) {
            ui_set_sleep_mode(false);
            hal_set_brightness(200);
            screen_on = true;
        }
    }

    // Refresh UI cada segundo (solo si pantalla activa)
    if (now - last_ui_refresh > 1000 && screen_on) {
        update_display_data();
        last_ui_refresh = now;
    }

    // Lógica vPet cada 10 segundos
    if (now - last_logic_refresh > 10000) {
        process_vpet_logic();
        last_logic_refresh = now;
    }

    lv_timer_handler();
#ifndef ESP32
    SDL_Delay(5);
#endif
}

#ifndef ESP32
int main(int argc, char **argv) { setup(); while (1) loop(); return 0; }
#endif