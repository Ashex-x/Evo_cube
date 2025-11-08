#define I2S_DOUT      17
#define I2S_BCLK      4
#define I2S_LRC       1
#include <Audio.h>
Audio audio;

void initPlay() {
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(18);
}


void playTrack(int track);       // 已有的声明可以保留

const char *serverBase = "http://192.168.19.212:13579/";
const char *musicFiles[] = 
{
  "China.mp3",
  "Sailing for.mp3", 
  "Fireworks cool easily.mp3",
  "solar flare.mp3",
  "Phantom of the opera.mp3"
};

int currentTrack = 0;
int totalTracks = sizeof(musicFiles) / sizeof(musicFiles[0]);



void playTrack(int track)
{
  if(track >= 0 && track < totalTracks)
  {
    audio.stopSong();
    currentTrack = track;
    
    String fullURL = String(serverBase) + String(musicFiles[track]);

    audio.connecttohost(fullURL.c_str());
  }

}