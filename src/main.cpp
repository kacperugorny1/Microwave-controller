#include <Arduino.h>
#define INC_DOWN 22 // INCREMENT DOWN
#define INC_UP 23 // INCREMENT UP
#define START_READ 1 // START BUTTON
#define OUT1 3 // step
#define OUT2 21 // impulse

#define LED1 19
#define LED2 18
#define LED3 5
#define LED4 17
int LED[4] = {LED1, LED2, LED3, LED4};
uint8_t seconds = 0;
uint8_t history_up = 0;
uint8_t history_down = 0;

void ShowNum();

void setup() {
  Serial.begin(115200);
  pinMode(INC_DOWN ,INPUT_PULLDOWN);
  pinMode(INC_UP ,INPUT_PULLDOWN);
  pinMode(START_READ ,INPUT_PULLDOWN);

  //OUTPUTS STATE LOW
  pinMode(OUT1, OUTPUT);
  digitalWrite(OUT1,LOW);
  pinMode(OUT2, OUTPUT);
  digitalWrite(OUT2,LOW);

  //LED STATE LOW
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1,LOW);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2,LOW);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED3,LOW);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED4,LOW);
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
    ShowNum();
    
  }
  if(history_down == 0b11110000){
    if(seconds != 0)
      seconds--;
    Serial.println(seconds);
    ShowNum();
  }
  return;
  go:
  //step
  digitalWrite(OUT1, HIGH);
  while(seconds > 0){
    delay(1000);
    seconds--;
    Serial.println(seconds);
    ShowNum();
  }
  digitalWrite(OUT1, LOW);

  // impulse
  digitalWrite(OUT2, HIGH);
  delay(1000);
  digitalWrite(OUT2,LOW);
}

void ShowNum(){
  if(seconds > 15)
  {
    digitalWrite(LED[0], HIGH);
    digitalWrite(LED[1], HIGH);
    digitalWrite(LED[2], HIGH);
    digitalWrite(LED[3], HIGH);
    return;
  }
  for(int i = 0; i < 4; i++){
    if((seconds >> i & 0b0001) == 0b0001)
      digitalWrite(LED[i], HIGH);
    else
      digitalWrite(LED[i], LOW);
  }
}