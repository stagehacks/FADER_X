#include "Fader.h"
#include <Arduino.h>
//#include <FastLED.h>

extern byte globalPage;
extern byte ledPageHues[4];
extern int globalFaderTargets[255];
extern byte globalFaderChannels[32];
extern void touchEvent(Fader* fader);

extern unsigned short globalMessageWaitMillis;
extern int globalMotorMinSpeed;
extern int globalMotorSpeedScale;
extern unsigned short globalMotorFrequency;
extern boolean globalRotated;
extern byte globalMotherboardRevision;
extern long globalLastBoot;

static byte PINS_1_A[8] = {0, 2, 4, 6, 8, 10, 24, 28};
static byte PINS_1_B[8] = {1, 3, 5, 7, 9, 12, 25, 29};
static byte PINS_2_A[8] = {1, 2, 5, 6, 9, 10, 36, 28};
static byte PINS_2_B[8] = {0, 3, 4, 7, 8, 12, 37, 29};


boolean firstSetup = true;

#define FMODE_Disabled -1
#define FMODE_Rest 0
#define FMODE_Touch 1
#define FMODE_Motor 2
#define FMODE_Pause 3
#define FMODE_Stall 4

Fader::Fader(int r, int realIndex){
  this->readPin = r;
  this->realIndex = realIndex;
}

void Fader::setup(byte index){
  if(globalMotherboardRevision==1){
    this->motorPinA = PINS_1_A[this->realIndex];
    this->motorPinB = PINS_1_B[this->realIndex];
  }else if(globalMotherboardRevision==2){
    this->motorPinA = PINS_2_A[this->realIndex];
    this->motorPinB = PINS_2_B[this->realIndex];
  }
  
  pinMode(this->motorPinA, OUTPUT);
  pinMode(this->motorPinB, OUTPUT);
  pinMode(this->readPin, INPUT_PULLUP);

  if(analogRead(this->readPin)>4085){
    this->mode = FMODE_Disabled;
  }

  analogWriteFrequency(motorPinA, globalMotorFrequency);
  analogWriteFrequency(motorPinB, globalMotorFrequency);

  this->index = index;
  this->updateChannel();
  this->rawPosition = analogRead(this->readPin);
}


void Fader::loop(){
  if(this->mode == FMODE_Disabled){ return; }
  long mils = millis();
  
  if(mils-globalLastBoot < 100){
    this->rawPosition = analogRead(this->readPin);
    globalFaderTargets[this->channel] = this->getPosition();
    return;
  }
  
  int distanceToTarget = abs(this->getPosition()-globalFaderTargets[this->channel]);
  if(globalFaderTargets[this->channel]==0 && this->getPosition()<0){
    distanceToTarget = 0;
  }
  if(globalFaderTargets[this->channel]==1023 && this->getPosition()>1023){
    distanceToTarget = 0;
  }

  boolean timedOut = mils-this->lastUpdate > max(200, globalMessageWaitMillis);

  switch(this->mode){
    case FMODE_Rest:
      if(abs(this->rawPosition-analogRead(this->readPin))>40){
        this->mode = FMODE_Touch;
        this->lastUpdate = mils;
        this->lastModeStart = mils;
  //      Serial.print(this->realIndex);
  //      Serial.println(" touched!");
        
      }else if(distanceToTarget>15){
        this->mode = FMODE_Motor;
        this->lastUpdate = mils;
        this->lastModeStart = mils;
        
      }
      analogWrite(this->motorPinA, 0);
      analogWrite(this->motorPinB, 0);
      break;

      
    case FMODE_Touch:
      if(timedOut){
        touchEvent(this);
        this->mode = FMODE_Rest;
        this->lastModeStart = mils;
      }else{
        this->rawPosition = analogRead(this->readPin);
        this->touchLoop(mils);
      }
      break;


    case FMODE_Motor:
      if(timedOut){
        this->mode = FMODE_Rest;
        this->lastModeStart = mils;
        
      }else if(mils-this->lastModeStart>1000){
        if(abs(globalFaderTargets[this->channel]-this->getPosition())>20){
          this->mode = FMODE_Stall;
          this->lastModeStart = mils;
        }else{
          this->mode = FMODE_Rest;
          this->lastModeStart = mils;
        }
      }else{
        this->rawPosition = analogRead(this->readPin);
        this->motorLoop(mils);
      }
      break;


    case FMODE_Stall:
      if(abs(this->rawPosition-analogRead(this->readPin))>200){
        this->mode = FMODE_Touch;
        this->lastUpdate = mils;
      }
      analogWrite(this->motorPinA, 0);
      analogWrite(this->motorPinB, 0);
      break;


    case FMODE_Pause:
      analogWrite(this->motorPinA, 0);
      analogWrite(this->motorPinB, 0);
      break;
    
  }

  
  if(this->mode <= FMODE_Motor && this->mode>FMODE_Rest && mils-this->lastUpdate > max(200, globalMessageWaitMillis)){
    if(this->mode == FMODE_Touch){
      touchEvent(this); // tail debounce
    }
    this->mode = FMODE_Rest;
    this->lastModeStart = mils;
  }

}


void Fader::touchLoop(long mils){
  if(globalFaderTargets[this->channel]!=this->getPosition() && mils-this->lastTouchEvent>globalMessageWaitMillis){
    this->lastTouchEvent = mils;
    touchEvent(this);
  }

  globalFaderTargets[this->channel] = this->getPosition();
  
}


int prevTarget = 0;
void Fader::motorLoop(long mils){

  int target = globalFaderTargets[this->channel];
  int distance = target-this->getPosition();
  int speed = min(globalMotorMinSpeed+abs(distance)/globalMotorSpeedScale, 180);

  if(prevTarget!=target){
    this->lastModeStart = mils;
  }
  prevTarget = target;

  if(globalRotated){
    distance = distance*-1;
  }

  if(distance>8){
    analogWrite(this->motorPinA, speed);
    analogWrite(this->motorPinB, 0);
    this->lastUpdate = mils;
  }else if(distance<-8){
    analogWrite(this->motorPinA, 0);
    analogWrite(this->motorPinB, speed);
    this->lastUpdate = mils;
  }else{
    analogWrite(this->motorPinA, 0);
    analogWrite(this->motorPinB, 0);
  }
  
}

int Fader::getPositionTrimmed(){
  return min(max(this->getPosition(), 0), 1023);
}
int Fader::getPosition(){
  if(globalRotated){
    return map(this->rawPosition, 300, 3900, 1023, 0);
  }else{
    return map(this->rawPosition, 300, 3900, 0, 1023);
  }
  
}
void Fader::pause(){
  this->mode = 3;
}
void Fader::unpause(){
  this->mode = 0;
}
void Fader::updateChannel(){
  this->channel = globalFaderChannels[this->index+8*globalPage];
}
byte Fader::getChannel(){
  return this->channel;
}
int Fader::getMode(){
  return this->mode;
}
void Fader::setMode(int m){
  this->mode = m;
  this->motorLoop(millis());
}
