#include "app_manager.h"
#include "ui.h"

// ── Apps registradas ──────────────────────────────────────────────────────
void app_fitness_launch();  void app_fitness_destroy();  void app_fitness_tick();
void app_pokedex_launch();  void app_pokedex_destroy();  void app_pokedex_tick();
void app_pomodoro_launch(); void app_pomodoro_destroy(); void app_pomodoro_tick();

static const App registry[] = {
    { app_fitness_launch,  app_fitness_destroy,  app_fitness_tick,  "fitness"  },
    { app_pokedex_launch,  app_pokedex_destroy,  app_pokedex_tick,  "pokedex"  },
    { app_pomodoro_launch, app_pomodoro_destroy, app_pomodoro_tick, "pomodoro" },
};
static const int REGISTRY_SIZE = sizeof(registry) / sizeof(registry[0]);

// ── Estado interno ────────────────────────────────────────────────────────
static const App *current_app = nullptr;

// ── Implementación ────────────────────────────────────────────────────────
void app_manager_init() {
    current_app = nullptr;
}

void app_manager_launch(const char *id) {
    if (current_app) app_manager_exit();
    for (int i = 0; i < REGISTRY_SIZE; i++) {
        if (strcmp(registry[i].id, id) == 0) {
            current_app = &registry[i];
            current_app->launch();
            return;
        }
    }
}

void app_manager_exit() {
    if (!current_app) return;
    const App *app = current_app;
    current_app = nullptr;
    // Primero nulamos punteros, luego animamos.
    // del_prev=true: LVGL borra la pantalla del app al terminar la animación.
    app->destroy();
    lv_scr_load_anim(ui_get_shell_screen(),
                     LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, true);
}

void app_manager_tick() {
    if (current_app && current_app->tick)
        current_app->tick();
}

bool app_manager_active() {
    return current_app != nullptr;
}