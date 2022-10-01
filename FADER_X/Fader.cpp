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

static byte PINS_1_A[8] = {0, 2, 4, 6, 8, 10, 24, 28};
static byte PINS_1_B[8] = {1, 3, 5, 7, 9, 12, 25, 29};
static byte PINS_2_A[8] = {1, 2, 5, 6, 9, 10, 36, 28};
static byte PINS_2_B[8] = {0, 3, 4, 7, 8, 12, 37, 29};

extern elapsedMillis globalClock;

boolean firstSetup = true;

#define FMODE_DISABLED -1
#define FMODE_REST 0
#define FMODE_TOUCH 1
#define FMODE_MOTOR 2
#define FMODE_PAUSE 3
#define FMODE_STALL 4

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
    this->mode = FMODE_DISABLED;
  }

  analogWriteFrequency(motorPinA, globalMotorFrequency);
  analogWriteFrequency(motorPinB, globalMotorFrequency);

  this->index = index;
  this->updateChannel();
  this->rawPosition = analogRead(this->readPin);
}


void Fader::loop(){
  if(this->mode == FMODE_DISABLED){ return; }

  if(globalClock<100){
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


  if(this->mode == FMODE_REST){
    if(abs(this->rawPosition-analogRead(this->readPin))>40){
      this->mode = FMODE_TOUCH;
      this->modeTimeout = 0;
//      Serial.print(this->realIndex);
//      Serial.println(" touched!");
      
    }else if(distanceToTarget>15){
      this->mode = FMODE_MOTOR;
      this->modeTimeout = 0;
      
    }
    analogWrite(this->motorPinA, 0);
    analogWrite(this->motorPinB, 0);
    
  }else if(this->mode == FMODE_STALL){
    if(abs(this->rawPosition-analogRead(this->readPin))>200){
      this->mode = FMODE_TOUCH;
      this->modeTimeout = 0;
    }
    analogWrite(this->motorPinA, 0);
    analogWrite(this->motorPinB, 0);
    
  }else if(this->mode == FMODE_PAUSE){
    analogWrite(this->motorPinA, 0);
    analogWrite(this->motorPinB, 0);
    
  }else{
    this->rawPosition = analogRead(this->readPin);
  }

  

  if(this->mode <= FMODE_MOTOR && this->modeTimeout>max(200, globalMessageWaitMillis)){
    if(this->mode == FMODE_TOUCH){
      touchEvent(this);
    }
    this->mode = FMODE_REST;
  }


  if(this->mode == FMODE_TOUCH){
    this->touchLoop();
  }else if(this->mode == FMODE_MOTOR){
    this->motorLoop();
  }
  

}


void Fader::touchLoop(){
  if(globalFaderTargets[this->channel]!=this->getPosition() && this->touchEventTimeout>globalMessageWaitMillis){
    this->touchEventTimeout = 0;
    touchEvent(this);
  }

  globalFaderTargets[this->channel] = this->getPosition();
  
}



void Fader::motorLoop(){

  int target = globalFaderTargets[this->channel];
  int distance = target-this->getPosition();
  int speed = min(globalMotorMinSpeed+abs(distance)/globalMotorSpeedScale, 180);

  if(globalRotated){
    distance = distance*-1;
  }

  if(distance>8){
    analogWrite(this->motorPinA, speed);
    analogWrite(this->motorPinB, 0);
    this->modeTimeout = 0;
  }else if(distance<-8){
    analogWrite(this->motorPinA, 0);
    analogWrite(this->motorPinB, speed);
    this->modeTimeout = 0;
  }else{
    analogWrite(this->motorPinA, 0);
    analogWrite(this->motorPinB, 0);
  }

  if(abs(prevDistance-distance)>16){
    prevDistance = distance;
    this->lastActualMove = millis();
  }

  if(millis()-lastActualMove>2000){
    this->mode = FMODE_STALL;
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
  this->motorLoop();
}
