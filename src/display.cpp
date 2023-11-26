#include <display.h>
const int nums[11][7] = {{SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGF}, {SEGB, SEGC, SEGB, SEGC, SEGB, SEGC, SEGB},
                  {SEGA, SEGB, SEGG, SEGE, SEGD, SEGD, SEGD}, {SEGA,SEGB,SEGG,SEGC,SEGD,SEGC,SEGD},
                  {SEGF, SEGB, SEGG, SEGC,SEGB, SEGG, SEGC}, {SEGA,SEGF,SEGG,SEGC,SEGD,SEGC,SEGD},
                  {SEGA,SEGF,SEGG,SEGC,SEGD,SEGE,SEGE}, {SEGA,SEGB,SEGC,SEGA,SEGB,SEGC,SEGA},
                  {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG}, {SEGA,SEGB,SEGG,SEGF,SEGC,SEGD,SEGD},
                  {SEGG,SEGG,SEGG,SEGG,SEGG,SEGG,SEGG}};
const int SEG[7] = {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG};
const int LED_DISPLAY[4] = {LED1 ,LED2, LED3, LED4};
void Configure(){
    //LED displays
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  
  for(int seg : SEG){
    pinMode(seg, OUTPUT_OPEN_DRAIN);
    digitalWrite(seg, HIGH);
  }
}

void ShowNum(int display_port, int num, bool dot){
    digitalWrite(display_port, HIGH);
      for(int seg : nums[num])
      {
        digitalWrite(seg, LOW);
      }
      digitalWrite(DOT, dot?LOW:HIGH);
      delay(1);
      digitalWrite(DOT, HIGH);
      for(int seg : SEG)
      {
        digitalWrite(seg, HIGH);
      }
      digitalWrite(display_port, LOW);
}

void ShowNums(void * parameter){ //task
  while(true){
    int seconds = *(int*)parameter;
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