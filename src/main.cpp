#include <Arduino.h>
#define INC_DOWN 22 // INCREMENT DOWN
#define INC_UP 23 // INCREMENT UP
#define START_READ 1 // START BUTTON
#define OUT1 3 // step
#define OUT2 21 // impulse
// malfunction ports 34 35 36 39
#define LED1 16 // dont work
#define LED2 32
#define LED3 33
#define LED4 27

#define SEGA 26
#define SEGB 14 // dont work
#define SEGC 17 // dont work
#define SEGD 18 
#define SEGE 19
#define SEGF 25
#define SEGG 5 // dont work
#define DOT 4

int LED_DISPLAY[4] = {LED2, LED3, LED4, LED1};
int SEG[7] = {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG};
int nums[10][7] = {{SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGF}, {SEGB, SEGC, SEGB, SEGC, SEGB, SEGC, SEGB},
                   {SEGA, SEGB, SEGG, SEGE, SEGD, SEGD, SEGD}, {SEGA,SEGB,SEGG,SEGC,SEGD,SEGC,SEGD},
                   {SEGF, SEGB, SEGG, SEGC,SEGB, SEGG, SEGC}, {SEGA,SEGF,SEGG,SEGC,SEGD,SEGC,SEGD},
                   {SEGA,SEGF,SEGG,SEGC,SEGD,SEGE,SEGE}, {SEGA,SEGB,SEGC,SEGA,SEGB,SEGC,SEGA},
                   {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG}, {SEGA,SEGB,SEGG,SEGF,SEGC,SEGD,SEGD}
                    };
int seconds = 0;
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
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  for(int seg : SEG){
    pinMode(seg, OUTPUT_OPEN_DRAIN);
    digitalWrite(seg, HIGH);
  }
  pinMode(DOT, OUTPUT_OPEN_DRAIN);
  digitalWrite(DOT, HIGH);
  
  
}


void loop() {
  //button start
  if(digitalRead(START_READ) == HIGH && seconds != 0){
      //step
    digitalWrite(OUT1, HIGH);
    ulong now = millis();
    while(seconds != 0){
      if(millis() - now > 1000){
        seconds--;
        now = millis();
      }
      ShowNum();
    }
    digitalWrite(OUT1, LOW);

    // impulse
    digitalWrite(OUT2, HIGH);
    delay(1000);
    digitalWrite(OUT2,LOW);
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
    if(seconds != 599)
      seconds++;
  }
  if(history_down == 0b11110000){
    if(seconds != 0)
      seconds--;
  }
  ShowNum();
  
  
}

void ShowNum(){
  int mins = seconds / 60;
  int onlyseconds = seconds % 60;
  if(mins != 0){
    digitalWrite(LED_DISPLAY[2], HIGH);
    for(int seg : nums[mins])
    {
      digitalWrite(seg, LOW);
    }
    digitalWrite(DOT, LOW);
    delay(1);
    digitalWrite(DOT, HIGH);
    for(int seg : SEG)
    {
      digitalWrite(seg, HIGH);
    }
    digitalWrite(LED_DISPLAY[2], LOW);
  }


  digitalWrite(LED_DISPLAY[1], HIGH);
  for(int seg : nums[onlyseconds/10])
  {
    digitalWrite(seg, LOW);
  }
  delay(1);
  for(int seg : SEG)
  {
    digitalWrite(seg, HIGH);
  }
  digitalWrite(LED_DISPLAY[1], LOW);
  


  digitalWrite(LED_DISPLAY[0], HIGH);
  for(int seg : nums[onlyseconds%10])
  {
    digitalWrite(seg, LOW);
  }
  delay(1);
  for(int seg : SEG)
  {
    digitalWrite(seg, HIGH);
  }
  
  digitalWrite(LED_DISPLAY[0], LOW);





} 