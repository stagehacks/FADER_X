#include <Arduino.h>
#include <QNEthernet.h>
#include "Fader.h"
#include "OSC.h"
using namespace qindesign::network;

#ifndef Eos_H
#define Eos_H

class Eos {
  private:
    

  public:
    void setup();
    void loop();

    EthernetClient eosClient;
    
    void touchEvent(int channel, Fader *fader);
    void knobEvent(byte channel, Encoder *encoder);
    void motorEvent(int channel, Fader *fader);
    void processOSC(OSCMessage msg);
    
    byte faderBank = 1;
    byte faderCount = 40;
    boolean connected = false;

};

#endif
