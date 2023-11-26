#pragma once
#include <Arduino.h>
// malfunction ports 34 35 36 39 - it input only
// pick Seg display
#define LED1 16 
#define LED2 32
#define LED3 33
#define LED4 27

#define SEGA 26
#define SEGB 14 
#define SEGC 17
#define SEGD 18 
#define SEGE 19
#define SEGF 25
#define SEGG 5
#define DOT 4

extern const int LED_DISPLAY[];
extern const int SEG[];
extern const int nums[][7];

void ShowNums(void * parameter);
void ShowNum(int,int,bool);
void Configure();