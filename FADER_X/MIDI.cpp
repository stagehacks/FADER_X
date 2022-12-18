#include "MIDI.h"

extern void setFaderTarget(byte index, int value);
int knobValues[255];
extern Fader fader1, fader2, fader3, fader4, fader5, fader6, fader7, fader8;
extern Encoder encoders[8];

void midiCCHandle(byte channel, byte fader, byte value) {
  setFaderTarget(fader, value*8);
  Serial.println(value);
}

void Midi::setup(){
  usbMIDI.setHandleControlChange(midiCCHandle);
  Serial.print("MIDI Setup on channel ");
  Serial.println(this->sendChannel);

  fader1.label(fader1.getPositionTrimmed()/8);
  fader2.label(fader2.getPositionTrimmed()/8);
  fader3.label(fader3.getPositionTrimmed()/8);
  fader4.label(fader4.getPositionTrimmed()/8);
  fader5.label(fader5.getPositionTrimmed()/8);
  fader6.label(fader6.getPositionTrimmed()/8);
  fader7.label(fader7.getPositionTrimmed()/8);
  fader8.label(fader8.getPositionTrimmed()/8);

  for(byte i=0; i<8; i++){
    encoders[i].label(0);
  }
 
}

void Midi::loop(){
  usbMIDI.read(this->listenChannel);
}



void Midi::touchEvent(int channel, Fader *fader){
  byte pos = fader->getPositionTrimmed()/8;
  usbMIDI.sendControlChange(channel, pos, this->sendChannel);

  fader->label(pos);
  
}

void Midi::knobEvent(byte channel, Encoder *encoder){
  knobValues[channel] += encoder->direction;
  knobValues[channel] = min(127, max(0, knobValues[channel]));
  usbMIDI.sendControlChange(channel, knobValues[channel], this->sendChannel);

  encoder->label(knobValues[channel]);
  
}

void Midi::motorEvent(int channel, Fader *fader){
  fader->label(fader->getTarget()/8);
}
