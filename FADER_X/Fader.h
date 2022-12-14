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
    long lastTouchEvent = 0;
    long lastMotorEvent = 0;
    long lastModeStart = 0;
    int lastTouchValue = 0;
    int lastStartPosition = 0;
    int easeSpeed = 0;
    int lastTarget = 0;
 
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
    void setTargetToCurrentPosition();
    int getPositionTrimmed();
    int getPosition();
    int getMode();
    byte getChannel();
    int getTarget();
    
    void label(String text);
    
    byte index = 0;
    byte realIndex = 0;
    byte channel = 0;
    
};

class Encoder {

  private:
    
    
  public:
    void label(String text);

    int value = 0;
    int channel = 0;
    int realIndex = 0;
    int direction = 0;
  
};
