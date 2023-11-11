#include <Arduino.h>
#include <IRremote.h>
#include <WiFi.h>
#include <time.h>
#define INC_DOWN 22 // INCREMENT DOWN
#define INC_UP 23 // INCREMENT UP
#define START_READ 1 // START BUTTON
#define OUT1 3 // step
#define OUT2 21 // impulse

// malfunction ports 34 35 36 39 - it input only
// pick Seg display
#define LED4 32
#define LED3 33
#define LED2 25
#define LED1 16

//encoder paths
#define A 15 
#define B 0 

#define BCD0 19
#define BCD1 17
#define BCD2 5
#define BCD3 18

// #define SEGA 26
// #define SEGB 14 
// #define SEGC 17
// #define SEGD 18 
// #define SEGE 19
// #define SEGF 25
// #define SEGG 5
#define DOT 4

#define IRRead 39
const char* ssid = "internet_dom1";
const char* password = "1qaz2wsx";
const char* ntpServer = "tempus1.gum.gov.pl";
const char* ntpServer2 = "tempus2.gum.gov.pl";
const String timezone = 	"CET-1CEST,M3.5.0,M10.5.0/3";

int LED_DISPLAY[4] = {LED1 ,LED2, LED3, LED4};
// int SEG[7] = {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG};
// int nums[11][7] = {{SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGF}, {SEGB, SEGC, SEGB, SEGC, SEGB, SEGC, SEGB},
//                    {SEGA, SEGB, SEGG, SEGE, SEGD, SEGD, SEGD}, {SEGA,SEGB,SEGG,SEGC,SEGD,SEGC,SEGD},
//                    {SEGF, SEGB, SEGG, SEGC,SEGB, SEGG, SEGC}, {SEGA,SEGF,SEGG,SEGC,SEGD,SEGC,SEGD},
//                    {SEGA,SEGF,SEGG,SEGC,SEGD,SEGE,SEGE}, {SEGA,SEGB,SEGC,SEGA,SEGB,SEGC,SEGA},
//                    {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG}, {SEGA,SEGB,SEGG,SEGF,SEGC,SEGD,SEGD},
//                    {SEGG,SEGG,SEGG,SEGG,SEGG,SEGG,SEGG}};
int nums[11][3] = {{10,10,10},{BCD0,10,10},{BCD1,10,10},{BCD0,BCD1,10},{BCD2,10,10},{BCD2,BCD0,10}
                  ,{BCD2,BCD1,10},{BCD2,BCD1,BCD0},{BCD3,10,10},{BCD3,BCD0,10}};
int seconds = 0;
uint8_t history_up = 0;
uint8_t history_down = 0;
bool didBChange = true;
int previousA;
int previousB;
bool start = false;

void ShowNums(void * parameter);
void ShowNum(int,int,bool);
void ReadingInput(void * parameter);
void Encoder_Inc();
void setTimezone(String timezone);

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


  //OUTPUTS STATE LOW
  pinMode(OUT1, OUTPUT);
  digitalWrite(OUT1,LOW);
  pinMode(OUT2, OUTPUT);
  digitalWrite(OUT2,LOW);

  //LED displays
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);


  // for(int seg : SEG){
  //   pinMode(seg, OUTPUT_OPEN_DRAIN);
  //   digitalWrite(seg, HIGH);
  // }
  pinMode(BCD0,OUTPUT);
  digitalWrite(BCD0,LOW);
  pinMode(BCD1,OUTPUT);
  digitalWrite(BCD1,LOW);
  pinMode(BCD2,OUTPUT);
  digitalWrite(BCD2,LOW);
  pinMode(BCD3,OUTPUT);
  digitalWrite(BCD3,LOW);
  
  pinMode(DOT, OUTPUT_OPEN_DRAIN);

  digitalWrite(DOT, HIGH);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  auto time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if(millis() - time == 1000*10)
      break;
    delay(500);
  }
  if(WiFi.status() == WL_CONNECTED){
    configTime(0, 0, ntpServer, ntpServer2);
    setTimezone(timezone);
  }

  IrReceiver.begin(IRRead);
  attachInterrupt(A, Encoder_Inc, FALLING);
  xTaskCreatePinnedToCore(
      ShowNums, /* Function to implement the task */
      "Task_Display", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task_Display,  /* Task handle. */
      0); /* Core where the task should run */
  xTaskCreatePinnedToCore(
      ReadingInput, /* Function to implement the task */
      "Task_Input", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &Task_Input,  /* Task handle. */
      1); /* Core where the task should run */



  
}

void loop() {
  //button start
  if((digitalRead(START_READ) == HIGH || start) && seconds != 0){
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
  start = false;
}

void ReadingInput(void * parameter){ //task
  while(true){
    history_down = history_down << 1;
    history_up = history_up << 1;

    if(IrReceiver.decode()){
      IrReceiver.resume();
      switch (IrReceiver.decodedIRData.command){
        case 67:
          start = true;
        break;
        case 25:
          if(seconds < 575)
            seconds+=30;
          else
            seconds=599;
          break;
        case 22:
          seconds = 0;
          break;
        case 7:
          if(seconds != 0)
            seconds--;
          break;
        case 21:
          if(seconds != 599)
            seconds++;
          break;
      } 

    }

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

void setTimezone(String timezone){
  setenv("TZ",timezone.c_str(),1);
  tzset();
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

void ShowNum(int display_port, int num, bool dot){
    digitalWrite(display_port, HIGH);
      for(int seg : nums[num])
      {
        digitalWrite(seg, HIGH);
        // digitalWrite(seg, LOW);
      }
      digitalWrite(DOT, dot?LOW:HIGH);
      delay(1);
      digitalWrite(DOT, HIGH);
      digitalWrite(BCD0,LOW);
      digitalWrite(BCD1,LOW);
      digitalWrite(BCD2,LOW);
      digitalWrite(BCD3,LOW);
      
      // for(int seg : SEG)
      // {
      //   digitalWrite(seg, HIGH);
      // }
      digitalWrite(display_port, LOW);
}

void ShowNums(void * parameter){ //task
  while(true){
    if(seconds == 0){
      struct tm timeinfo;
      if(!getLocalTime(&timeinfo))
        break;
      ShowNum(LED_DISPLAY[0], timeinfo.tm_min%10, false);
      ShowNum(LED_DISPLAY[1], timeinfo.tm_min/10, false);
      ShowNum(LED_DISPLAY[2], timeinfo.tm_hour%10, true);
      ShowNum(LED_DISPLAY[3], timeinfo.tm_hour/10, false);
    }

    int mins = seconds / 60;
    if(mins != 0)
      ShowNum(LED_DISPLAY[3], mins, true);  
    if(mins > 0 || seconds >= 10)
      ShowNum(LED_DISPLAY[2], (seconds % 60)/10, false);
    if(seconds > 0 || mins > 0)
      ShowNum(LED_DISPLAY[1], (seconds % 60)%10, false);
  }
} 