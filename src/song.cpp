#include <song.h>

void playMusic() {
  for (int i=0;i<203;i++){              //203 is the total number of music notes in the song
    int wait = duration[i] * songspeed;
    tone(buzzer,notes[i],wait);          //tone(pin,frequency,duration)
    delay(wait);
  }                        //delay is used so it doesn't go to the next loop before tone is finished playing
  //You can click reset on Arduino to replay the song
}

