#include "MIDI.h"

extern void setFaderTarget(byte index, int value);

extern void proEncoderLabel(byte index, String text);
extern void proFaderLabel(byte index, String text);

int knobValues[255];



void midiCCHandle(byte channel, byte fader, byte value) {
  setFaderTarget(fader, value*8);
}

void Midi::setup(){
  usbMIDI.setHandleControlChange(midiCCHandle);
  Serial.print("MIDI Setup on channel ");
  Serial.println(this->sendChannel);
 
}

void Midi::loop(){
  usbMIDI.read(this->listenChannel);
}



void Midi::touchEvent(int channel, Fader *fader){
  byte pos = fader->getPositionTrimmed()/8;
  usbMIDI.sendControlChange(channel, pos, this->sendChannel);

  fader->proLabel(pos);
  
}

void Midi::knobEvent(byte channel, Encoder *encoder){
  knobValues[channel] += encoder->direction;
  knobValues[channel] = min(127, max(0, knobValues[channel]));
  usbMIDI.sendControlChange(channel, knobValues[channel], this->sendChannel);

  encoder->proLabel(knobValues[channel]);
  
}
