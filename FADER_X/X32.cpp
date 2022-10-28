#include "X32.h"
#include "Net.h"

extern Net net;
extern void setFaderTarget(byte index, int value);
extern byte globalFaderChannels[32];

byte initialIndex = 0;
long lastInitialRequestAttempt = 0;
boolean receivedFaderValues = false;
int lastRequestedInitChannel;
long lastSubscription = 0;


void X32::setup(){
  Serial.println("X32 Setup");
  udp.begin(10023);

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

    udp.beginPacket(net.IP_Destination, 10023);
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
  char ch[2];
  sprintf(ch, "%02i", channel); //leftpad with 0

  String oscStr = "";

  if(xTarget == 1){
    oscStr = "/ch/"+String(ch)+"/mix/fader";
  }else if(xTarget == 2){
    oscStr = "/dca/"+String(channel)+"/fader";
  }else if(xTarget == 3){
    oscStr = "/bus/"+String(ch)+"/mix/fader";
  }else if(xTarget == 4){
    oscStr = "/auxin/"+String(ch)+"/mix/fader";
  }else if(xTarget == 5){
    oscStr = "/fxrtn/"+String(ch)+"/mix/fader";
  }else if(xTarget == 6){
    oscStr = "/mtx/"+String(ch)+"/mix/fader"; 
  }
  OSCMessage msg(oscStr);
  msg.addFloat(fader->getPositionTrimmed()/1023.0);
  
  udp.beginPacket(net.IP_Destination, 10023);
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
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(xTarget == 2 && msg.match("/dca/*/fader")){
    byte channel = msg.addressPart(1).toInt();
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;  
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(xTarget == 3 && msg.match("/bus/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(xTarget == 4 && msg.match("/auxin/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(xTarget == 5 && msg.match("/fxrtn/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    if(channel == lastRequestedInitChannel){
      initialRequest();
    }
    
  }else if(xTarget == 6 && msg.match("/mtx/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
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

  udp.beginPacket(net.IP_Destination, 10023);
  msg.writeUDP(&udp);
  udp.endPacket();
}


void X32::initialRequest(){

  if(initialIndex>=32){
    lastRequestedInitChannel = 999;
    return;
  }
  lastRequestedInitChannel = globalFaderChannels[initialIndex];

  char ch[2];
  sprintf(ch, "%02i", globalFaderChannels[initialIndex]); //leftpad with 0
  String oscStr = "";

  if(xTarget == 1 && globalFaderChannels[initialIndex]<=32){
    oscStr = "/ch/"+String(ch)+"/mix/fader";
  }else if(xTarget == 2 && globalFaderChannels[initialIndex]<=8){
    oscStr = "/dca/"+String(globalFaderChannels[initialIndex])+"/fader";
  }else if(xTarget == 3 && globalFaderChannels[initialIndex]<=16){
    oscStr = "/bus/"+String(ch)+"/mix/fader";
  }else if(xTarget == 4 && globalFaderChannels[initialIndex]<=8){
    oscStr = "/auxin/"+String(ch)+"/mix/fader";
  }else if(xTarget == 5 && globalFaderChannels[initialIndex]<=8){
    oscStr = "/fxrtn/"+String(ch)+"/mix/fader";
  }else if(xTarget == 6 && globalFaderChannels[initialIndex]<=8){
    oscStr = "/mtx/"+String(ch)+"/mix/fader"; 
  }


  if(oscStr!="" && globalFaderChannels[initialIndex]>0){
    //Serial.print("sent: ");
    //Serial.println(oscStr);
    OSCMessage msg(oscStr);
    udp.beginPacket(net.IP_Destination, 10023);
    msg.writeUDP(&udp);
    udp.endPacket();
  }

  initialIndex++;
    
  
}
