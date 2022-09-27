#include "DiGiCo.h"
#include "Net.h"

extern Net net;
extern void setFaderTarget(byte index, int value);

void DiGiCo::setup(){
  Serial.println("DiGiCo setup");
  udp.begin(net.IP_DestinationPort);
  
}


void DiGiCo::loop(){

  int size = udp.parsePacket();
  if (size > 0 && size <= sizeof(buf)) {
    udp.read(buf, size);
    OSCMessage msgIn(buf, size);
    processOSC(msgIn);
  }
 
}


void DiGiCo::touchEvent(int channel, Fader *fader){
  OSCMessage msg("/Input_Channels/"+String(channel)+"/fader");
  msg.addInt(this->linearToLog(fader->getPositionTrimmed()));

  udp.beginPacket(net.IP_Destination, net.IP_DestinationPort);
  msg.writeUDP(&udp);
  udp.endPacket();
}


void DiGiCo::processOSC(OSCMessage msg){
  if(msg.match("/Input_Channels/*/fader")){
    setFaderTarget(msg.addressPart(1).toInt(), logToLinear(msg.getFloat(0)));
  }
}


int DiGiCo::linearToLog(int val){
  int mappedVal = 0;
  if (val >= 0 && val < 100) {
    mappedVal = map(val, 0, 100, -150, -60);
  } else if (val >= 50 && val < 512) {
    mappedVal = map(val, 50, 512, -60, -20);
  } else if (val >= 512) {
    mappedVal = map(val, 512, 1023, -20, 10);
  }
  return mappedVal;
}

int DiGiCo::logToLinear(float val){
  int mappedVal = 0;
  if (val >= -150 && val < -60) {
    mappedVal = map(val, -150, -60, 0, 100);
  } else if (val >= -60 && val < -20) {
    mappedVal = map(val, -60, -20, 100, 512);
  } else if (val >= -20) {
    mappedVal = map(val, -20, 10, 512, 1023);
  }
  return mappedVal;
}
