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

#define A 15
#define B 0

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
bool didBChange = true;
int previousA;
int previousB;

void ShowNum(void * parameter);
void ReadingInput(void * parameter);
void Encoder_Inc();

void setup() {
  Serial.begin(115200);
  TaskHandle_t Task_Display;
  TaskHandle_t Task_Input;
  pinMode(INC_DOWN ,INPUT_PULLDOWN);
  pinMode(INC_UP ,INPUT_PULLDOWN);
  pinMode(START_READ ,INPUT_PULLDOWN);

  pinMode(A, INPUT_PULLUP);
  previousA = digitalRead(A);
  pinMode(B, INPUT_PULLUP);
  previousB = digitalRead(B);

  attachInterrupt(A, Encoder_Inc, FALLING);

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

  

  xTaskCreatePinnedToCore(
      ShowNum, /* Function to implement the task */
      "Task_Display", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &Task_Display,  /* Task handle. */
      0); /* Core where the task should run */
  xTaskCreatePinnedToCore(
      ReadingInput, /* Function to implement the task */
      "Task_Input", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &Task_Display,  /* Task handle. */
      1); /* Core where the task should run */
  
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
    }
    digitalWrite(OUT1, LOW);
    // impulse
    digitalWrite(OUT2, HIGH);
    delay(1000);
    digitalWrite(OUT2,LOW);
  }
}

void ReadingInput(void * parameter){ //task
  while(true){
    history_down = history_down << 1;
    history_up = history_up << 1;
    if(digitalRead(INC_DOWN) == HIGH){
      history_down = history_down + 1;
    }

    if(digitalRead(INC_UP) == HIGH){
      history_up = history_up + 1;
    }
    if(history_up == 0b11110000){
      if(seconds < 590)
        seconds+=10;
    }
    if(history_down == 0b11110000){
      if(seconds >= 10)
        seconds-=10;
      else
        seconds = 0;
    }
    
    if(previousB != digitalRead(B))
      didBChange = true;
    previousB = digitalRead(B);
    previousA = digitalRead(A);
  }
}

void Encoder_Inc(){ //przerwanie
  if(didBChange && previousA == HIGH && digitalRead(A) == LOW){
    if(digitalRead(B) == LOW){
      if(seconds != 599)
        seconds++;
    }
    else{
      if(seconds != 0)
        seconds--;
    } 
      
    didBChange = false;
  }
}

void ShowNum(void * parameter){ //task
  while(true){

    int mins = seconds / 60;
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
    if(mins > 0 || seconds >= 10)
    for(int seg : nums[(seconds % 60)/10])
    {
      digitalWrite(seg, LOW);
    }
    delay(1);
    for(int seg : SEG)
    {
      digitalWrite(seg, HIGH);
    }
    digitalWrite(LED_DISPLAY[1], LOW);
    

    if(seconds > 0 || mins > 0)
    digitalWrite(LED_DISPLAY[0], HIGH);
    for(int seg : nums[(seconds % 60)%10])
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
} 