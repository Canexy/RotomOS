#pragma once
#include "system_data.h"

// Carga el estado guardado en myStatus.
// Si no hay save válido, deja los valores por defecto (first_boot=true).
void persistence_load();

// Guarda el estado actual de myStatus.
void persistence_save();