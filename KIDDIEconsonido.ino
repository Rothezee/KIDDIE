/*
Version 2.0 de KIDDIE Junio del 2025

- "calesita" de canciones
- Control de Pantalla TFT MCUFRIEND
- Control de memoria SD que trae el display
- Sistema embebido con EEPROM
*/

#include <SD.h>
#include <SPI.h>
#include <TMRpcm.h>
#include <EEPROM.h>
#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>

// Pines
#define pinSD        53    // CS tarjeta SD
#define AUDIO_PIN    46    // Salida audio PWM
#define COIN_PIN     36    // Entrada moneda
#define SEL_PIN      32    // Botón selector de canción
#define BUTTON_IN    30    // Entrada botón SBOTON
#define SBOTON_PIN   28    // Salida SBOTON
#define SMOTOR_PIN   38    // Salida motor
#define SW1_PIN      34    // DIP switch 
#define SW2_PIN      35    // DIP switch 
#define LUZ1_PIN     22    
#define LUZ2_PIN     24    
#define LUZ3_PIN     26    

// Colores TFT
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F

// Duraciones conocidas (ms)
const unsigned long DUR_SALUDO = 5000UL;             // 0:05
const unsigned long DUR_THEMES[3] = {147000UL,       // 2:27
                                     176000UL,       // 2:56
                                      82000UL};      // 1:22

// Tiempos de juego según DIP switches
unsigned long TIEMPO_JUEGO = 83000UL;  // Variable, se configura en setup()

// Librerías
TMRpcm Audio;
MCUFRIEND_kbv tft;

// Estados de reproducción
enum State { IDLE, PLAY_SALUDO, PLAY_THEME };
State state = IDLE;

// Variables de control
unsigned int CONTADORCOIN = 0;
unsigned long gameStart = 0;   
unsigned long songStart = 0;   // Inicio de la canción actual
int cancionSeleccionada = 1;   //
int LA = 0;

void setup() {
  Serial.begin(9600);

  // Configurar pines
  pinMode(COIN_PIN, INPUT_PULLUP);
  pinMode(SEL_PIN, INPUT_PULLUP);
  pinMode(BUTTON_IN, INPUT_PULLUP);
  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(SMOTOR_PIN, OUTPUT);
  pinMode(SBOTON_PIN, OUTPUT);
  pinMode(LUZ1_PIN, OUTPUT);
  pinMode(LUZ2_PIN, OUTPUT);
  pinMode(LUZ3_PIN, OUTPUT);

  // Configurar tiempo según DIP switches
  if (digitalRead(SW1_PIN) == HIGH && digitalRead(SW2_PIN) == HIGH) {
    TIEMPO_JUEGO = 83000UL;   // 1:23
  }
  if (digitalRead(SW1_PIN) == HIGH && digitalRead(SW2_PIN) == LOW) {
    TIEMPO_JUEGO = 105000UL;  // 1:45
  }
  if (digitalRead(SW1_PIN) == LOW && digitalRead(SW2_PIN) == HIGH) {
    TIEMPO_JUEGO = 135000UL;  // 2:15
  }
  if (digitalRead(SW1_PIN) == LOW && digitalRead(SW2_PIN) == LOW) {
    TIEMPO_JUEGO = 165000UL;  // 2:45
  }

  // Audio
  Audio.speakerPin = AUDIO_PIN;
  Audio.setVolume(5);

  // TFT
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(10,10);
  tft.print("Calesita v2025");

  // Mostrar tiempo configurado
  mostrarTiempo();

  // Cargar creditos desde EEPROM
  EEPROM.get(0, CONTADORCOIN);
  mostrarContador();

  // Iniciar SD
  if (!SD.begin(pinSD)) {
    tft.setCursor(10,40);
    tft.setTextColor(RED);
    tft.print("Error SD");
    while (true);
  }
}

void loop() {
  unsigned long now = millis();

  // Detección de moneda
  static bool lastCoin = HIGH;
  bool coinState = digitalRead(COIN_PIN);
  if (coinState == LOW && lastCoin == HIGH) {
    CONTADORCOIN++;
    EEPROM.put(0, CONTADORCOIN);
    mostrarContador();
    if (state == IDLE && CONTADORCOIN > 0) {
      state = PLAY_SALUDO;
      gameStart = now;        // Marca inicio de juego
      songStart = now;        // Marca inicio de saludo
      Audio.play("saludo.wav");
    }
  }
  lastCoin = coinState;

  // Selector de canción: puede cambiar durante PLAY_THEME sin afectar gameStart
  static bool lastSel = HIGH;
  bool selState = digitalRead(SEL_PIN);
  if (selState == LOW && lastSel == HIGH) {
    cancionSeleccionada++;
    if (cancionSeleccionada > 3) cancionSeleccionada = 1;
    if (state == PLAY_THEME) {
      playSong(cancionSeleccionada);
      songStart = now;  // reinicia conteo solo para duración de esta canción
    }
  }
  lastSel = selState;

  // Lógica de estado
  if (state == PLAY_SALUDO) {
    if (now - songStart >= DUR_SALUDO) {
      state = PLAY_THEME;
      songStart = now;
      playSong(cancionSeleccionada);
      digitalWrite(SMOTOR_PIN, HIGH);
    }
  }
  else if (state == PLAY_THEME) {
    // Duración de la canción actual
    if (now - songStart >= DUR_THEMES[cancionSeleccionada - 1]) {
      Audio.stopPlayback();
      state = IDLE;
      digitalWrite(SMOTOR_PIN, LOW);
      CONTADORCOIN--;
      EEPROM.put(0, CONTADORCOIN);
      mostrarContador();
    }
    // Fin de tiempo de juego (TIEMPO VARIABLE según DIP switches)
    if (now - gameStart >= TIEMPO_JUEGO) {
      Audio.stopPlayback();
      state = IDLE;
      digitalWrite(SMOTOR_PIN, LOW);
      CONTADORCOIN--;
      EEPROM.put(0, CONTADORCOIN);
      mostrarContador();
    }
  }

  // SBOTON refleja BUTTON_IN
  digitalWrite(SBOTON_PIN, digitalRead(BUTTON_IN));

    // Secuencia de luces con intervalos de ~1 segundo usando millis()
static unsigned long prevBlink = 0;
static int blinkPhase = 0;  // 0:Luz1 on, 1:Luz2 on, 2:Luz1 off, 3:Luz2 off
unsigned long interval = 1000;  // Duración de cada fase en ms (~1 segundo)

if (millis() - prevBlink >= interval) {
  prevBlink = millis();
  blinkPhase = (blinkPhase + 1) % 4;
  // Fases para LUZ1 y LUZ2
  digitalWrite(LUZ1_PIN, (blinkPhase == 0 || blinkPhase == 1));
  digitalWrite(LUZ2_PIN, (blinkPhase == 1 || blinkPhase == 2));
  // LUZ3 parpadea al mismo ritmo que LUZ1 pero en fases 0 y 2
  digitalWrite(LUZ3_PIN, (blinkPhase == 0 || blinkPhase == 2));
}
}

// Funciones auxiliares
void mostrarContador() {
  tft.fillRect(0, 100, 240, 40, BLACK);
  tft.setCursor(10, 110);
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.print("Coin: ");
  tft.print(CONTADORCOIN);
}

void mostrarTiempo() {
  tft.fillRect(0, 70, 240, 30, BLACK);
  tft.setCursor(10, 75);
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.print("Tiempo: ");
  tft.print(TIEMPO_JUEGO / 1000);
  tft.print("s");
}

void playSong(int num) {
  const char* files[3] = {"tema1.wav", "tema2.wav", "tema3.wav"};
  Audio.play(files[num - 1]);
}
