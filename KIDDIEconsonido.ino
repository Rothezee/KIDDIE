//vercion funcionando marzo 2023, instalada en calesita
//PIN 50 /MISO, PIN 51 /MOSI, PIN 52 /SCK, PIN 53 /CS
//PIN 11 SALIDA DE AUDIO, PIN 22 Y 24 SELECCION AUDIO,

#include <SD.h>
#include <SPI.h>
#include <TMRpcm.h>

TMRpcm Audio;
#define pinSD 53
#define USE_TIMER2

int ENTRADA1 = 22;  //estos pines para seleccion de audio en este eskech no se estan usando
int ENTRADA2 = 24;  // junio 2025, actualmente no se estan usando era una funcion para cambiar la musica mientras jugaba
int valor = 0;


int A = 0;
boolean B = 0;
boolean C = 0;

unsigned int CONTADORCOIN = 0;
unsigned long CONT = 0;
int BOTON = 4;    //entrada boton
int SW1 = 5;      //entrada dip swich seleccionar tiempo
int SW2 = 6;      // entrada dip swich seleccionar tiempo
int COIN = 8;     //entrada coin
int SMOTOR = 10;  //salida motor
int SBOTON = 28;  //salida activada por boton para usar luz o bocina

int LA = 0;     //variable axiliar luces
int LUZ1 = 22;  //salidas luces
int LUZ2 = 24;
int LUZ3 = 26;

unsigned int TIEMPO = 0;  //esta variable se usa para asignar el tiempo segun los dip swich

int CREDIT = 0;  //variable para asignar creditos para jugar

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
  Audio.speakerPin = 11;
  Serial.begin(9600);

  if (!SD.begin(pinSD)) {
    Serial.println("falla en sd");
    return;
  }

  CREDIT = 1;

  if (digitalRead(SW1) == HIGH && digitalRead(SW2) == HIGH) { TIEMPO = 83000; }
  if (digitalRead(SW1) == HIGH && digitalRead(SW2) == LOW) { TIEMPO = 105000; }
  if (digitalRead(SW1) == LOW && digitalRead(SW2) == HIGH) { TIEMPO = 135000; }
  if (digitalRead(SW1) == LOW && digitalRead(SW2) == LOW) { TIEMPO = 165000; }
}

void loop() {
  // put your main code here, to run repeatedly


  B = digitalRead(COIN);
  if (B == LOW && C != B) {
    C = LOW;
    CONTADORCOIN++;
    Serial.println(CONTADORCOIN);
    delay(200);
  }
  if (B == HIGH && B != C) { C = HIGH; }
  if (CONTADORCOIN >= CREDIT) { CONT++; }
  delayMicroseconds(150);

  if (CONT == 100) { Audio.play("coin1"); }

  if (CONT == 2800) { Audio.play("music"); }

  if (CONT == 3000) {
    ;
    digitalWrite(SMOTOR, HIGH);
  }

  if (digitalRead(BOTON == HIGH)) { digitalWrite(SBOTON, HIGH); }
  if (digitalRead(BOTON == LOW)) { digitalWrite(SBOTON, LOW); }


  if (CONT > TIEMPO) {
    digitalWrite(SMOTOR, LOW);
    Audio.stopPlayback();
    CONTADORCOIN = (CONTADORCOIN - CREDIT);
    CONT = 0;
    delay(3000);
  }

  if (LA == 1) { digitalWrite(LUZ1, HIGH); }
  if (LA == 500) { digitalWrite(LUZ2, HIGH); }
  if (LA == 1000) { digitalWrite(LUZ1, LOW); }
  if (LA == 1500) { digitalWrite(LUZ2, LOW); }
  if (LA == 1) { digitalWrite(LUZ3, HIGH); }
  if (LA == 500) { digitalWrite(LUZ3, LOW); }

  LA++;

  if (CONT == 0) { delayMicroseconds(900); }

  if (LA == 2000) {
    LA = 0;
    Serial.println(CONT);
  }
}
