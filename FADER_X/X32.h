#include <Arduino.h>
#include <QNEthernet.h>
#include "Fader.h"
#include "OSC.h"
using namespace qindesign::network;

#ifndef X32_H
#define X32_H

class X32 {
  private:
    EthernetUDP udp;
    char buf[Ethernet.mtu() - 20 - 8];
    void initialRequest();
    
  public:
    void setup();
    void loop();
    
    void touchEvent(int channel, Fader *fader);
    void processOSC(OSCMessage msg);
    void updateSubscription();

    byte xTarget = 1;

};

#endif
