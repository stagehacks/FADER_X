#include <Arduino.h>
#include <QNEthernet.h>
using namespace qindesign::network;

#ifndef OSC_H
#define OSC_H

union oscBlob {
   int i;
   float f;
   unsigned char b[4];
   int s[2];
};

class OSCMessage{

  private:
    void byteSLIP(EthernetClient *client, unsigned char b);
    void nullPad(EthernetClient *client, byte count);
    void nullPadUDP(EthernetUDP *udp, byte count);
    
    String addr;
    oscBlob blobs[4];
   
    byte parameterCount = 0;
    char paramTypes[4];
    String paramStrings[6];

  public:
    OSCMessage(char* buf, int length);
    OSCMessage(String address);
    
    String getAddress();
    int getInt(byte pos);
    float getFloat(byte pos);
    String getString(byte pos);
    String addressPart(byte part);
    boolean match(String test);
    void addFloat(float p);
    void addInt(int p);
    void addString(String p);
    void writeSLIP(EthernetClient *client);
    void writeUDP(EthernetUDP *udp);
};

#endif
