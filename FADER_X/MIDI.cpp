#include "MIDI.h"

extern int globalFaderTargets[255];

void midiCCHandle(byte channel, byte fader, byte value) {
  globalFaderTargets[fader] = value*8;
}

void Midi::setup(){
  usbMIDI.setHandleControlChange(midiCCHandle);
}

void Midi::loop(){
  usbMIDI.read(this->listenChannel);
}

void Midi::touchEvent(int channel, Fader *fader){
  Serial.println("midi touch");
  usbMIDI.sendControlChange(channel, fader->getPositionTrimmed()/8, this->sendChannel);
}
