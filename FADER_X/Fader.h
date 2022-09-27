#include <Arduino.h>
#pragma once

extern int globalFaderTargets[255];

class Fader {
  
  private:
    int rawPosition = 0;
    int motorPinA;
    int motorPinB;
    int readPin;
    int mode = 0;

    elapsedMillis modeTimeout = 0;
    elapsedMillis touchEventTimeout = 0;    
 
  public:
    Fader(int r, int realIndex);    

    void setup(byte index);
    void loop();

    void touchLoop();
    void motorLoop();
    void ledUpdate();
    void pause();
    void unpause();
    void tick();
    void setTarget(int position);
    void setMode(int m);
    void updateChannel();
    int getPositionTrimmed();
    int getPosition();
    int getMode();
    byte getChannel();
    
    byte index = 0;
    byte realIndex = 0;
    byte channel = 0;
    
};
