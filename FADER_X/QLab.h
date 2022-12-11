#include <Arduino.h>
#include <QNEthernet.h>
#include "Fader.h"
#include "OSC.h"

using namespace qindesign::network;

#ifndef QLab_H
#define QLab_H

class QLab {
  private:
    EthernetUDP udp;
    

  public:
    void setup();
    void loop();
    
    void touchEvent(int channel, Fader *fader);
    void motorEvent(int channel, Fader *fader);
    void heartbeat();
    void parseOSC(OSCMessage msg);
    void changePage();
    boolean doMidiBehavior();

    char passcode[8];
    boolean midiEnabled = false;
    boolean autoMidi = false;
    int maximumVolume = 12;
    int minimumVolume = 60;
    int faderTargetsOSC[65];
    int faderTargetsMIDI[128];
    
};

#endif
