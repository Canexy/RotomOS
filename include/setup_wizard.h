#pragma once

// Lanza el wizard de primer arranque.
// Al terminar llama a persistence_save() y arranca el OS shell.
void setup_wizard_start();