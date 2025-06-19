#include <SD.h>
#include <SPI.h>
#include <TMRpcm.h>
#include <EEPROM.h>
#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>

TMRpcm Audio;
#define pinSD 53
#define USE_TIMER2

MCUFRIEND_kbv tft;

int ENTRADA1 = 42;
int ENTRADA2 = 44;

int A = 0;
boolean B = 0;
boolean C = 0;

unsigned int CONTADORCOIN = 0;
unsigned long CONT = 0;

int BOTON = 30;
int SW1 = 32;
int SW2 = 34;
int COIN = 36;
int SMOTOR = 38;
int SBOTON = 40;

int LA = 0;
int LUZ1 = 22;
int LUZ2 = 24;
int LUZ3 = 26;

unsigned int TIEMPO = 0;
int CREDIT = 0;

void mostrarContador() {
  tft.fillRect(0, 100, 240, 40, BLACK);
  tft.setCursor(10, 110);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("Monedas: ");
  tft.print(CONTADORCOIN);
}

void setup() {
  Serial.begin(9600);

  pinMode(COIN, INPUT_PULLUP);
  pinMode(SMOTOR, OUTPUT);
  pinMode(LUZ1, OUTPUT);
  pinMode(LUZ2, OUTPUT);
  pinMode(LUZ3, OUTPUT);
  pinMode(BOTON, INPUT_PULLUP);
  pinMode(SBOTON, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(ENTRADA1, INPUT_PULLUP);
  pinMode(ENTRADA2, INPUT_PULLUP);

  Audio.speakerPin = 46;
  Audio.setVolume(5);

  // TFT
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Sistema Iniciado");

  // Leer contador de EEPROM
  EEPROM.get(0, CONTADORCOIN);
  mostrarContador();

  // Inicializar SD
  if (!SD.begin(pinSD)) {
    Serial.println("Falla en SD");
    tft.setCursor(10, 40);
    tft.setTextColor(RED);
    tft.print("SD Error");
    return;
  }

  CREDIT = 1;

  if (digitalRead(SW1) == HIGH && digitalRead(SW2) == HIGH) { TIEMPO = 83000; }
  if (digitalRead(SW1) == HIGH && digitalRead(SW2) == LOW)  { TIEMPO = 105000; }
  if (digitalRead(SW1) == LOW  && digitalRead(SW2) == HIGH) { TIEMPO = 135000; }
  if (digitalRead(SW1) == LOW  && digitalRead(SW2) == LOW)  { TIEMPO = 165000; }
}

void loop() {
  B = digitalRead(COIN);
  if (B == LOW && C != B) {
    C = LOW;
    CONTADORCOIN++;
    EEPROM.put(0, CONTADORCOIN);
    mostrarContador();
    delay(200);
  }
  if (B == HIGH && B != C) { C = HIGH; }

  if (CONTADORCOIN >= CREDIT) { CONT++; }
  delayMicroseconds(150);

  if (CONT == 100) {
    Audio.play("coin1");
  }

  if (CONT == 2800) {
    Audio.play("music");
  }

  if (CONT == 3000) {
    digitalWrite(SMOTOR, HIGH);
  }

  if (digitalRead(BOTON) == HIGH) {
    digitalWrite(SBOTON, HIGH);
  } else {
    digitalWrite(SBOTON, LOW);
  }

  if (CONT > TIEMPO) {
    digitalWrite(SMOTOR, LOW);
    Audio.stopPlayback();
    CONTADORCOIN -= CREDIT;
    EEPROM.put(0, CONTADORCOIN);
    mostrarContador();
    CONT = 0;
    delay(3000);
  }

  if (LA == 1)    digitalWrite(LUZ1, HIGH);
  if (LA == 500)  digitalWrite(LUZ2, HIGH);
  if (LA == 1000) digitalWrite(LUZ1, LOW);
  if (LA == 1500) digitalWrite(LUZ2, LOW);
  if (LA == 1)    digitalWrite(LUZ3, HIGH);
  if (LA == 500)  digitalWrite(LUZ3, LOW);

  LA++;

  if (CONT == 0) {
    delayMicroseconds(900);
  }

  if (LA == 2000) {
    LA = 0;
  }
}
