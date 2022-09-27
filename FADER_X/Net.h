#include <Arduino.h>
#include <QNEthernet.h>
#include "OSC.h"
#include "QLab.h"
using namespace qindesign::network;


class Net {
  
  private:
    byte pin;
    uint8_t macAddress[6];

  public:
    void loadEEPROM();
    void setup();
    void loop();

    byte getIP_Static(byte index);
    byte getIP_Subnet(byte index);
    byte getIP_Gateway(byte index);
    byte getIP_Destination(byte index);
    unsigned short getIP_SelfPort();
    unsigned short getIP_DestinationPort();
    byte getMAC(byte index);
    
    IPAddress IP_Static{0, 0, 0, 0};
    IPAddress IP_Subnet{0, 0, 0, 0};
    IPAddress IP_Gateway{0, 0, 0, 0};
    IPAddress IP_Destination{0, 0, 0, 0};

    int netMode = 0;
    unsigned short IP_SelfPort = 0;
    unsigned short IP_DestinationPort = 0;
};
