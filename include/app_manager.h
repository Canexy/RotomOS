#pragma once
#include <lvgl.h>

// ── Interfaz que toda app debe implementar ────────────────────────────────
struct App {
    void (*launch)  ();        // construye la pantalla y la carga
    void (*destroy) ();        // limpia al salir (llamado automáticamente)
    void (*tick)    ();        // actualización periódica (puede ser nullptr)
    const char *id;            // nombre único, p.ej. "fitness"
};

// ── API pública del gestor ────────────────────────────────────────────────
void app_manager_init   ();
void app_manager_launch (const char *id);   // lanza app por id
void app_manager_exit   ();                 // vuelve al OS shell
void app_manager_tick   ();                 // llamar desde loop()
bool app_manager_active ();                 // true si hay app abierta