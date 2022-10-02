#include "X32.h"
#include "Net.h"

extern Net net;
extern void setFaderTarget(byte index, int value);

void X32::setup(){
  Serial.println("X32 setup");
  subscribed = false;
  udp.begin(10023);
  
  updateSubscription();

}


void X32::loop(){

  if(!subscribed && millis()-lastSubscription>9000){
    lastSubscription = millis();
    updateSubscription();
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
  sprintf(ch, "%02i", channel);
  OSCMessage msg("/ch/"+String(ch)+"/mix/fader");
  msg.addFloat(fader->getPositionTrimmed()/1023.0);
  
  udp.beginPacket(net.IP_Destination, 10023);
  msg.writeUDP(&udp);
  udp.endPacket();

  Serial.print("sent: ");
  Serial.println(msg.getAddress());
}


void X32::processOSC(OSCMessage msg){
  String oscAddr = msg.getAddress();

  //Serial.print("revd: ");
  //Serial.println(oscAddr);

  if(msg.match("/ch/*/mix/fader")){
    byte channel = msg.addressPart(1).toInt();
    Serial.print(channel);
    Serial.print("=");
    Serial.println(msg.getFloat(0));
    setFaderTarget(channel, msg.getFloat(0)*1023);
  }

}


void X32::updateSubscription(){
  OSCMessage msg("/xremote");
//  msg.addString("/faders");
//  msg.addString("/ch/**/mix/fader");
//  msg.addInt(1);
//  msg.addInt(32);
//  msg.addInt(2);

  udp.beginPacket(net.IP_Destination, 10023);
  msg.writeUDP(&udp);
  udp.endPacket();

  Serial.println("updated sub");
}
