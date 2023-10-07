#include <Arduino.h>
#define INC_DOWN 22 // INCREMENT DOWN
#define INC_UP 23 // INCREMENT UP
#define START_READ 1 // START BUTTON
#define OUT1 3 // step
#define OUT2 21 // impulse
uint8_t seconds = 0;
uint8_t history_up = 0;
uint8_t history_down = 0;
void setup() {
  Serial.begin(115200);
  pinMode(INC_DOWN ,INPUT_PULLDOWN);
  pinMode(INC_UP ,INPUT_PULLDOWN);
  pinMode(START_READ ,INPUT_PULLDOWN);
  pinMode(OUT1, OUTPUT);
  digitalWrite(OUT1,LOW);
  pinMode(OUT2, OUTPUT);
  digitalWrite(OUT2,LOW);
}


void loop() {
  if(digitalRead(START_READ) == HIGH && seconds != 0){
    goto go;
  }
  history_down = history_down << 1;
  history_up = history_up << 1;
  if(digitalRead(INC_DOWN) == HIGH){
    history_down = history_down + 1;
  }

  if(digitalRead(INC_UP) == HIGH){
    history_up = history_up + 1;
  }


  if(history_up == 0b11110000){
    seconds++;
    Serial.println(seconds);
    
  }
  if(history_down == 0b11110000){
    if(seconds != 0)
      seconds--;
    Serial.println(seconds);
  }
  return;
  go:
  digitalWrite(OUT1, HIGH);
  while(seconds > 0){
    delay(1000);
    seconds--;
    Serial.println(seconds);
  }
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, HIGH);
  delay(1000);
  digitalWrite(OUT2,LOW);
}

