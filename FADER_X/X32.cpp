#include "X32.h"
#include "Net.h"

extern Net net;
extern void setFaderTarget(byte index, int value);
extern byte globalFaderChannels[32];

void X32::setup(){
  Serial.println("X32 Setup");
  udp.begin(10023);
  
  updateSubscription();


  
}

boolean receivedFaderValues = false;
long lastInitialRequestAttempt = 0;
long lastSubscription = 0;

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
    oscStr = "/dca/"+String(channel)+"/mix/fader";
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

  //Serial.print("revd: ");
  //Serial.println(oscAddr);

  if(xTarget == 1 && msg.match("/ch/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    
  }else if(xTarget == 2 && msg.match("/dca/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    
  }else if(xTarget == 3 && msg.match("/bus/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    
  }else if(xTarget == 4 && msg.match("/auxin/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    
  }else if(xTarget == 5 && msg.match("/fxrtn/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    
  }else if(xTarget == 6 && msg.match("/mtx/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    setFaderTarget(channel, msg.getFloat(0)*1023);
    receivedFaderValues = true;
    
  }else if(msg.match("/info")){
    Serial.println("received info");
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
  
  for(byte i=0; i<32; i++){
    char ch[2];
    sprintf(ch, "%02i", globalFaderChannels[i]); //leftpad with 0

    String oscStr = "";
  
    if(xTarget == 1){
      oscStr = "/ch/"+String(ch)+"/mix/fader";
    }else if(xTarget == 2){
      oscStr = "/dca/"+String(globalFaderChannels[i])+"/mix/fader";
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
    udp.beginPacket(net.IP_Destination, 10023);
    msg.writeUDP(&udp);
    udp.endPacket();
    
  }
  
}
