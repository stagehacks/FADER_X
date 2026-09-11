#include "X32.h"
#include "X32Address.h"
#include "Net.h"

extern Net net;
extern void setFaderTarget(byte index, int value);
extern byte globalFaderChannels[32];
extern byte globalMode;

byte initialIndex = 0;
long lastInitialRequestAttempt = 0;
boolean receivedFaderValues = false;
int lastRequestedInitChannel;
long lastSubscription = 0;
int udpPort = 10023;

#define OP_X32 8
#define OP_XAIR 9


void X32::setup(){
  if(globalMode==OP_X32){
    Serial.println("X32 Setup");
    udpPort = 10023;
  }else{
    Serial.println("XAIR Setup");
    udpPort = 10024;
  }

  udp.begin(udpPort);
  
  initialIndex = 0;
  lastInitialRequestAttempt = 0;

  
}


void X32::loop(){

  if(net.linkOn && receivedFaderValues && millis()-lastSubscription>9000){
    lastSubscription = millis();
    updateSubscription();
  }

  if(!receivedFaderValues && net.linkOn && millis()-lastInitialRequestAttempt>2000){
    lastInitialRequestAttempt = millis();
    OSCMessage msg("/info");

    udp.beginPacket(net.IP_Destination, udpPort);
    msg.writeUDP(&udp);
    udp.endPacket();
    
  }

  int size = udp.parsePacket();
  if (size > 0 && size <= sizeof(buf)) {
    udp.read(buf, size);
    OSCMessage msgIn(buf, size);
    processOSC(msgIn);
  }
 
}


void X32::touchEvent(int channel, Fader *fader){
  char oscAddr[32];
  if(!x32FaderAddress(globalMode == OP_XAIR, xTarget, channel, oscAddr, sizeof(oscAddr))){
    Serial.println("Unsupported X32/X Air fader target");
    return;
  }
  OSCMessage msg(String(oscAddr));
  msg.addFloat(fader->getPositionTrimmed()/1023.0);
  
  udp.beginPacket(net.IP_Destination, udpPort);
  msg.writeUDP(&udp);
  udp.endPacket();

//  Serial.print("sent: ");
//  Serial.println(msg.getAddress());
}


void X32::processOSC(OSCMessage msg){
  String oscAddr = msg.getAddress();

//  Serial.print("revd: ");
//  Serial.println(oscAddr);

  if(xTarget == 1 && msg.match("/ch/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    if(!x32FaderTargetSupported(globalMode == OP_XAIR, xTarget, channel)) return;
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(xTarget == 2 && msg.match("/dca/*/fader")){
    byte channel = msg.addressPart(1).toInt();
    if(!x32FaderTargetSupported(globalMode == OP_XAIR, xTarget, channel)) return;
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;  
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(xTarget == 3 && msg.match("/bus/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    if(!x32FaderTargetSupported(globalMode == OP_XAIR, xTarget, channel)) return;
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(globalMode == OP_X32 && xTarget == 4 && msg.match("/auxin/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    if(!x32FaderTargetSupported(false, xTarget, channel)) return;
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(globalMode == OP_X32 && xTarget == 5 && msg.match("/fxrtn/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    if(!x32FaderTargetSupported(false, xTarget, channel)) return;
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(globalMode == OP_X32 && xTarget == 6 && msg.match("/mtx/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    if(!x32FaderTargetSupported(false, xTarget, channel)) return;
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(globalMode == OP_XAIR && xTarget == 4 && msg.match("/rtn/aux/mix/fader")){
    setFaderTarget(1, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(lastRequestedInitChannel == 1){
      initialRequest();
    }

  }else if(globalMode == OP_XAIR && xTarget == 5 && msg.match("/rtn/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    if(!x32FaderTargetSupported(true, xTarget, channel)) return;
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }

  }else if(msg.match("/info")){
    initialRequest();
    updateSubscription();
  }

}


void X32::updateSubscription(){
  OSCMessage msg("/xremote");

  udp.beginPacket(net.IP_Destination, udpPort);
  msg.writeUDP(&udp);
  udp.endPacket();
}


void X32::initialRequest(){

  if(initialIndex>=32){
    lastRequestedInitChannel = 999;
    return;
  }
  lastRequestedInitChannel = globalFaderChannels[initialIndex];

  char oscAddr[32];
  if(x32FaderAddress(globalMode == OP_XAIR, xTarget, globalFaderChannels[initialIndex], oscAddr, sizeof(oscAddr))){
    //Serial.print("sent: ");
    //Serial.println(oscAddr);
    OSCMessage msg(String(oscAddr));
    udp.beginPacket(net.IP_Destination, udpPort);
    msg.writeUDP(&udp);
    udp.endPacket();
  }

  initialIndex++;
    
  
}
