#include <Arduino.h>
#include <QNEthernet.h>
#include "Fader.h"
#include "OSC.h"
using namespace qindesign::network;

#ifndef DiGiCo_H
#define DiGiCo_H

class DiGiCo {
  private:
    EthernetUDP udp;
    char buf[Ethernet.mtu() - 20 - 8];

  public:
    void setup();
    void loop();
    
    void touchEvent(int channel, Fader *fader);
    void processOSC(OSCMessage msg);
    void updateSubscription();

    int linearToLog(int val);
    int logToLinear(float val);

};

#endif
