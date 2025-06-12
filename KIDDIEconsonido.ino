// Adaptación del código original para Arduino MEGA + TFT 2.8" con SD integrada
// Mantiene los botones físicos y añade display para mostrar créditos, tiempo y estado

#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>

// Pines TFT 2.8" ILI9341 shield (Arduino MEGA)
#define LCD_CS    A3
#define LCD_CD    A2
#define LCD_WR    A1
#define LCD_RD    A0
#define LCD_RESET A4

// Lector SD integrado en shield
#define SD_CS     53

TMRpcm Audio;
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Pines originales
int COIN       = 8;    // Entrada coin
int BOTON      = 4;    // Botón físico
int SW1        = 5;    // DIP switch tiempo bit 1
int SW2        = 6;    // DIP switch tiempo bit 2
int SMOTOR     = 10;   // Salida motor
int SBOTON     = 28;   // Salida activada por botón

int LUZ1       = 22;   // Salidas luces
int LUZ2       = 24;
int LUZ3       = 26;

// Variables de control
unsigned int CONTADORCOIN = 0;
unsigned long CONT        = 0;
unsigned int TIEMPO       = 0;
const int CREDIT          = 1;
int LA                   = 0;

// Posiciones de texto en pantalla
const int X_CREDITOS = 10;
const int Y_CREDITOS = 20;
const int X_ESTADO   = 10;
const int Y_ESTADO   = 50;
const int X_TIEMPO   = 10;
const int Y_TIEMPO   = 80;

void setup() {
  Serial.begin(9600);

  // Inicializar audio
  Audio.speakerPin = 11; // Pin de salida audio

  // Pines IO
  pinMode(COIN, INPUT_PULLUP);
  pinMode(BOTON, INPUT_PULLUP);
  pinMode(SMOTOR, OUTPUT);
  pinMode(SBOTON, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(LUZ1, OUTPUT);
  pinMode(LUZ2, OUTPUT);
  pinMode(LUZ3, OUTPUT);

  // Inicializar TFT
  tft.reset();
  uint16_t id = tft.readID();
  tft.begin(id);
  tft.setRotation(1);
  tft.fillScreen(0x0000); // Negro
  tft.setTextSize(2);
  tft.setTextColor(0xFFFF); // Blanco

  // Saludo inicial
  tft.setCursor(X_CREDITOS, Y_CREDITOS);
  tft.println("Calesita v2025");

  // Inicializar SD
  if (!SD.begin(SD_CS)) {
    tft.setCursor(X_ESTADO, Y_ESTADO);
    tft.setTextColor(0xF800); // Rojo
    tft.println("Error lector SD");
    Serial.println("Falla en SD");
  } else {
    tft.setCursor(X_ESTADO, Y_ESTADO);
    tft.setTextColor(0x07E0); // Verde
    tft.println("SD OK");
  }

  // Configurar tiempo según DIP switches
  if (digitalRead(SW1) == HIGH && digitalRead(SW2) == HIGH) TIEMPO =  83000;
  if (digitalRead(SW1) == HIGH && digitalRead(SW2) == LOW)  TIEMPO = 105000;
  if (digitalRead(SW1) == LOW  && digitalRead(SW2) == HIGH) TIEMPO = 135000;
  if (digitalRead(SW1) == LOW  && digitalRead(SW2) == LOW)  TIEMPO = 165000;

  delay(1000);
  tft.fillScreen(0x0000);
}

void loop() {
  // Leer moneda
  static bool C = HIGH;
  bool B = digitalRead(COIN);
  if (B == LOW && C == HIGH) {
    // Moneda insertada
    CONTADORCOIN++;
    C = LOW;
    Serial.print("Monedas: "); Serial.println(CONTADORCOIN);
    // Mostrar créditos
    tft.fillRect(X_CREDITOS, Y_CREDITOS, 200, 20, 0x0000);
    tft.setCursor(X_CREDITOS, Y_CREDITOS);
    tft.setTextColor(0xFFFF);
    tft.print("Creditos: "); tft.print(CONTADORCOIN);
    delay(200);
  } else if (B == HIGH) {
    C = HIGH;
  }

  // Iniciar juego cuando alcanza crédito
  if (CONTADORCOIN >= CREDIT && CONT == 0) {
    CONT = millis();
    Audio.play("coin1.wav");
    delay(500);
    Audio.play("music.wav");
    digitalWrite(SMOTOR, HIGH);
    tft.fillRect(X_ESTADO, Y_ESTADO, 200, 20, 0x0000);
    tft.setCursor(X_ESTADO, Y_ESTADO);
    tft.setTextColor(0x07E0);
    tft.println("Jugando...");
  }

  // Actualizar tiempo restante
  if (CONT > 0) {
    unsigned long elapsed = millis() - CONT;
    if (elapsed <= TIEMPO) {
      int remaining = (TIEMPO - elapsed) / 1000;
      tft.fillRect(X_TIEMPO, Y_TIEMPO, 200, 20, 0x0000);
      tft.setCursor(X_TIEMPO, Y_TIEMPO);
      tft.setTextColor(0xFFE0);
      tft.print("Tiempo: "); tft.print(remaining); tft.println(" s");
    }
  }

  // Finalizar juego
  if (CONT > 0 && millis() - CONT > TIEMPO) {
    digitalWrite(SMOTOR, LOW);
    Audio.stopPlayback();
    CONT = 0;
    CONTADORCOIN -= CREDIT;
    tft.fillRect(X_ESTADO, Y_ESTADO, 200, 20, 0x0000);
    tft.setCursor(X_ESTADO, Y_ESTADO);
    tft.setTextColor(0xF800);
    tft.println("Terminado");
    delay(2000);
    tft.fillRect(X_TIEMPO, Y_TIEMPO, 200, 20, 0x0000);
  }

  // Control botón físico SBOTON
  if (digitalRead(BOTON) == HIGH) digitalWrite(SBOTON, HIGH);
  else                           digitalWrite(SBOTON, LOW);

  // Secuencia de luces
  if (LA == 0)      digitalWrite(LUZ1, HIGH);
  else if (LA == 500)  digitalWrite(LUZ2, HIGH);
  else if (LA == 1000) digitalWrite(LUZ1, LOW);
  else if (LA == 1500) digitalWrite(LUZ2, LOW);
  else if (LA == 1)    digitalWrite(LUZ3, HIGH);
  else if (LA == 500)  digitalWrite(LUZ3, LOW);

  LA++;
  if (LA >= 2000) LA = 0;

  delayMicroseconds(150);
}
