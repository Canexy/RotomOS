
#include "persistence.h"
#include <string.h>

// Número mágico para validar que el save es nuestro
static const uint32_t SAVE_MAGIC = 0x524F544D; // "ROTM"
static const uint8_t  SAVE_VER   = 1;

// Estructura de save — solo datos serializables, sin punteros
struct SaveData {
    uint32_t magic;
    uint8_t  version;
    uint16_t pkm_id;
    char     pkm_name[16];
    int32_t  level;
    uint32_t exp;
    int32_t  hunger;
    int32_t  happiness;
    uint32_t step_goal;
    uint8_t  first_boot; // 0 = false, 1 = true
};

// ══════════════════════════════════════════════════════════════════════════
// EMULADOR PC — guarda en archivo binario junto al ejecutable
// ══════════════════════════════════════════════════════════════════════════
#ifndef ESP32

#include <stdio.h>
static const char *SAVE_PATH = "rotomos_save.dat";

void persistence_load() {
    FILE *f = fopen(SAVE_PATH, "rb");
    if (!f) return; // sin save → valores por defecto (first_boot=true)

    SaveData sd;
    size_t read = fread(&sd, sizeof(SaveData), 1, f);
    fclose(f);

    if (read != 1 || sd.magic != SAVE_MAGIC || sd.version != SAVE_VER)
        return; // save corrupto o versión antigua → valores por defecto

    myStatus.pkm_id     = sd.pkm_id;
    memcpy(myStatus.pkm_name, sd.pkm_name, 16);
    myStatus.level      = (int)sd.level;
    myStatus.exp        = sd.exp;
    myStatus.hunger     = (int)sd.hunger;
    myStatus.happiness  = (int)sd.happiness;
    myStatus.step_goal  = sd.step_goal;
    myStatus.first_boot = (sd.first_boot != 0);
}

void persistence_save() {
    SaveData sd;
    sd.magic      = SAVE_MAGIC;
    sd.version    = SAVE_VER;
    sd.pkm_id     = myStatus.pkm_id;
    memcpy(sd.pkm_name, myStatus.pkm_name, 16);
    sd.level      = (int32_t)myStatus.level;
    sd.exp        = myStatus.exp;
    sd.hunger     = (int32_t)myStatus.hunger;
    sd.happiness  = (int32_t)myStatus.happiness;
    sd.step_goal  = myStatus.step_goal;
    sd.first_boot = myStatus.first_boot ? 1 : 0;

    FILE *f = fopen(SAVE_PATH, "wb");
    if (!f) return;
    fwrite(&sd, sizeof(SaveData), 1, f);
    fclose(f);
}

// ══════════════════════════════════════════════════════════════════════════
// HARDWARE REAL — usa Preferences (NVS) del ESP32
// ══════════════════════════════════════════════════════════════════════════
#else

#include <Preferences.h>
static Preferences prefs;
static const char *NS = "rotomos"; // namespace NVS (max 15 chars)

void persistence_load() {
    prefs.begin(NS, true); // true = solo lectura

    uint32_t magic = prefs.getUInt("magic", 0);
    uint8_t  ver   = prefs.getUChar("ver", 0);

    if (magic != SAVE_MAGIC || ver != SAVE_VER) {
        prefs.end();
        return; // sin save válido → valores por defecto
    }

    myStatus.pkm_id    = (uint16_t)prefs.getUInt("pkm_id", 789);
    prefs.getString("pkm_name", myStatus.pkm_name, 16);
    myStatus.level     = prefs.getInt("level",     1);
    myStatus.exp       = prefs.getUInt("exp",      0);
    myStatus.hunger    = prefs.getInt("hunger",    100);
    myStatus.happiness = prefs.getInt("happy",     100);
    myStatus.step_goal = prefs.getUInt("step_goal",5000);
    myStatus.first_boot= prefs.getBool("fboot",    true);

    prefs.end();
}

void persistence_save() {
    prefs.begin(NS, false); // false = lectura/escritura

    prefs.putUInt("magic",     SAVE_MAGIC);
    prefs.putUChar("ver",      SAVE_VER);
    prefs.putUInt("pkm_id",    myStatus.pkm_id);
    prefs.putString("pkm_name",myStatus.pkm_name);
    prefs.putInt("level",      myStatus.level);
    prefs.putUInt("exp",       myStatus.exp);
    prefs.putInt("hunger",     myStatus.hunger);
    prefs.putInt("happy",      myStatus.happiness);
    prefs.putUInt("step_goal", myStatus.step_goal);
    prefs.putBool("fboot",     myStatus.first_boot);

    prefs.end();
}

#endif