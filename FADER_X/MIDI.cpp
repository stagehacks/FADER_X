#include "MIDI.h"

extern void setFaderTarget(byte index, int value);

void midiCCHandle(byte channel, byte fader, byte value) {
  setFaderTarget(fader, value*8);
}

void Midi::setup(){
  usbMIDI.setHandleControlChange(midiCCHandle);
}

void Midi::loop(){
  usbMIDI.read(this->listenChannel);
}

void Midi::touchEvent(int channel, Fader *fader){
  usbMIDI.sendControlChange(channel, fader->getPositionTrimmed()/8, this->sendChannel);
}
