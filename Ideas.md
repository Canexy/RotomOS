hardware real:
- ESP32-S3R8, 1.43inch AMOLED Round Display Panel, QSPI Interface, 466x466 Resolution, 16.7M color, Xtensa 32-bits LX7 Dual-core Processor up to 240MHz, 2,4GHz Wi-fi, BLE 5, Onboard Antenna/Antenna Connector, Capacitive Touch controlled via I2C interface, PCF85063 RTC Chip with reserved SH1.0 RTC battery header for RTC function requirement, QMI8658 6-Axis IMU, TF Card Slot, Multiple interfaces, Built-in 512KB SRAM and 384KB ROM with onboard 16MB flash and 8MB PSRAM, onboard RST button and programmable BOOT button, onboard 3.7V MX1.25 lithium battery recharge/discharge header, USB Type-C port for everything, onboard UART and I2C SH1.0 4PIN connector reserved 2x14PIN 1.27mm pithc header adapting multiple GPIO interfaces
- Quiero conectarlo con una batería de litio 602530 500 mAh con JST1.25mm pins, de medidas lenght 30mm, width 25mm y high 6mm

como lo estoy simulando en el pc:
- uso PlatformIO en Visual Studio Code (ver todos los archivos de configuración)
- para cada prueba hago build y upload

idea principal, qué quiero:
- concepto: medallón/clip como el dispositivo pokéwalker que venia con pokémon heartgold y soulsilver, o reloj grande de pulsera (ver si cabe todo para que pueda ser reloj)
- ideas de funcionamiento: ver abajo
- setup de smartwatch: selección de hora, fecha, ** más cosas ** e inicio de OS (llamado RotomOS, basado en el Pokémon Rotom, conocido porque se adapta a dispositivos electronicos y adopta su forma)
- información de el por qué del proyecto: quería un vpet de pokémon y se adaptó a la idea actual. dicho vpet quiero que exista en este proyecto, pero en relación a todas las funciones que existirán en el mismo

qué tengo ahora: (repo)
- display emulado que se supone representa lo que se verá en físico (ver si cumple con el hardware real)
- funcionamiento táctil (adaptado para emulación)
- hora real con el sistema (será RTC en hardware real), fondo pantalla adaptado con la resolución usada, botón de prueba y 2 pantallas (al hacer slide)

qué cosas quiero que haga en el futuro:
- Sincronización Real: Que el clima en el juego sea el clima real de tu ciudad (vía API meteorológica).
- Batallas/Intercambio: Si dos personas tienen el dispositivo, pueden conectarse por Bluetooth (BLE) y pelear. *** rollo beacon
- Notificaciones: Que el LED RGB parpadee si tienes un correo o un mensaje en el móvil.
- Audio: Si le añades un pequeño altavoz I2S, ¡tendrás los gritos reales de los Pokémon! (ver si el modelo que tengo ya tiene altavoz)
- fitness: el pokémon solo evoluciona si yo memuevo (podómetro, si lo incluye), en un futuro: sincronización con móvil
- pomodoro viviente
- authenticator pa guardar 2FA ? súper *premium*, ver también si lee y reproduce nfc

- idea de arriba ** social hub: que si paso cerca de una persona con el mismo dispositivo, avisa pa pelear o intercambiar, si no aceptas eso, que registre su ficha de jugador/pokemon (rollo como se hacia en la 3ds con el streetpass) *premium* rollo beacon

- *premium* meterle actualizaciones ota, con eventos, mejoras, etc.
- sincronizacion con apis reales *** como la que he mencionado del clima arriba **
- quizás conexión con una pequeña web con los datos cuando sincronice  *premium*

- de todo lo mencionado arriba, usar funcionalidades wifi, bluetooth y todo lo que traiga el cacharro

qué debería ser lo primero:
- A elección y discusión contigo, Claude
- IMPORTANTE: todo esto es emulado ahora mismo, pero tendrá que ser adaptado en un futuro a hardware real para su funcionamiento. dejar código para ambos y eliminar la parte emulada cuando se pida dar el paso a hardware real