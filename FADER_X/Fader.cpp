#include "Fader.h"
#include <Arduino.h>
#include <Easing.h>

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

long mils;
boolean firstSetup = true;

#define FMODE_Disabled -1
#define FMODE_Rest 0
#define FMODE_Touch 1
#define FMODE_Motor 2
#define FMODE_Pause 3
#define EaseSpeed 400.0

Fader::Fader(int r, int realIndex){
  this->readPin = r;
  this->realIndex = realIndex;
}

EasingFunc<Ease::ExpoOut> ease;



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
  
  //Serial.println(analogRead(this->readPin));
  
  if(analogRead(this->readPin)>4085){
    setMode(FMODE_Disabled);
  }

  analogWriteFrequency(motorPinA, globalMotorFrequency);
  analogWriteFrequency(motorPinB, globalMotorFrequency);

  this->index = index;
  this->updateChannel();
  this->rawPosition = analogRead(this->readPin);

  ease.duration(EaseSpeed);
}



void Fader::loop(){
  if(this->mode == FMODE_Disabled){ return; }
  mils = millis();
  
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

  switch(this->mode){
    
    case FMODE_Rest:
      
      if(abs(this->rawPosition-analogRead(this->readPin))>128){
        setMode(FMODE_Touch); 
        
      }else if(distanceToTarget>30){
        setMode(FMODE_Motor);
        this->lastMotorEvent = mils;
        this->lastStartPosition = this->getPosition();
        
      }
      analogWrite(this->motorPinA, 0);
      analogWrite(this->motorPinB, 0);
      break;

      
    case FMODE_Touch:
      this->rawPosition = analogRead(this->readPin);
      this->touchLoop();
      break;


    case FMODE_Motor:
      this->rawPosition = analogRead(this->readPin);
      this->motorLoop();
      break;


    case FMODE_Pause:
      analogWrite(this->motorPinA, 0);
      analogWrite(this->motorPinB, 0);
      break;
    
  }

}


void Fader::touchLoop(){
  
  if(abs(globalFaderTargets[this->channel]-this->getPosition())>1 && mils-this->lastTouchEvent>globalMessageWaitMillis){
    globalFaderTargets[this->channel] = this->getPosition();
    this->lastTouchEvent = mils;
    touchEvent(this);
  }
  
  if(mils-this->lastTouchEvent > globalMessageWaitMillis*2 || mils-this->lastModeStart>300){ // tail debounce when after touching the fader
    globalFaderTargets[this->channel] = this->getPosition();
    touchEvent(this);
    setMode(FMODE_Rest);
    
  }
  
}

int prevTarget = 0;
void Fader::motorLoop(){

  int target = globalFaderTargets[this->channel];
  this->easeSpeed = 100+abs(target-this->lastStartPosition)/2;
  ease.duration(this->easeSpeed);
   
  if(prevTarget!=target){
    setMode(FMODE_Motor);
    this->lastStartPosition = this->getPosition();
    this->lastModeStart = mils;
  }
  prevTarget = target;

  if(mils-this->lastModeStart > max(this->easeSpeed+200, globalMessageWaitMillis)){
    setMode(FMODE_Rest);

  }else{
    int distance = target-this->getPosition();
    
    if(mils-this->lastModeStart < EaseSpeed && mils-this->lastModeStart>0){
      int scaledTarget = this->lastStartPosition+((target-this->lastStartPosition)*(ease.get(mils-this->lastModeStart)));
      distance = scaledTarget-this->getPosition();
    }
  
    if(globalRotated){
      distance = distance*-1;
    }

    int speed = min(globalMotorMinSpeed+abs(distance)/globalMotorSpeedScale, 255);

    if(distance>8){
      analogWrite(this->motorPinA, speed);
      analogWrite(this->motorPinB, 0);
      this->lastMotorEvent = mils;
    }else if(distance<-8){
      analogWrite(this->motorPinA, 0);
      analogWrite(this->motorPinB, speed);
      this->lastMotorEvent = mils;
    }else{
      analogWrite(this->motorPinA, 0);
      analogWrite(this->motorPinB, 0);
    }

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
  if(this->mode!=FMODE_Disabled){
    setMode(FMODE_Pause);
  }
}
void Fader::unpause(){
  if(this->mode!=FMODE_Disabled){
    setMode(FMODE_Rest);
  }
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
  if(this->mode != m){
    this->lastModeStart = millis();
  }
  this->mode = m;
}
