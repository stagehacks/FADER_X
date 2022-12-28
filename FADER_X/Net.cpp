#include "Net.h"
#include <QNEthernet.h>
#include <EEPROM.h>
using namespace qindesign::network;

extern EthernetServer globalWebServer;
extern EthernetUDP globalUDP;
extern void changePage(byte p);
extern Net net;

void Net::setup(){
  Ethernet.macAddress(this->macAddress);

  Ethernet.onLinkState([](bool state) {
    Serial.print("Ethernet Link state changed to ");
    Serial.println(state ? "ON":"OFF");
    randomSeed(millis()); 
    net.linkOn = state;
    
  });

  Ethernet.onAddressChanged([]() {
    Serial.println("Ethernet Address changed");
  });

  Serial.println("Beginning Ethernet");

  if(Ethernet.begin()){

    if(this->netMode == 2){
      if (!Ethernet.waitForLocalIP(5000)) {
        Serial.println("Unable to connect with DHCP");
        Serial.print("Changing to Static IP ");
        Serial.println(this->IP_Static);
        Ethernet.begin(this->IP_Static, this->IP_Subnet, this->IP_Gateway);
        
      } else{
        Serial.println("Connected via DHCP");
        Serial.println(this->IP_Static);
  
      }
    }else{
      Serial.print("Setting Static IP ");
      Serial.println(this->IP_Static);
      Ethernet.begin(this->IP_Static, this->IP_Subnet, this->IP_Gateway);
    }

    delay(100);

    globalWebServer.begin();
    Serial.println("Started Web Server");
    
    globalUDP.begin(29979);
    Serial.println("Started OSC Server :29979");
    
  }

  
}

extern EthernetUDP globalUDP;
extern byte globalPage;
extern byte globalMode;
extern byte globalOSCPageControl;
extern QLab qlab;
extern void channelUpdateAll();
char udpBuf[Ethernet.mtu() - 20 - 8];

extern void serveGET(EthernetClient client);

void Net::loop(){
  EthernetClient webClient = globalWebServer.available();

  if(webClient){
    serveGET(webClient);
    //webClient.flush();
    delay(2);
    webClient.closeOutput();
  }
  
  int udpPacketSize = globalUDP.parsePacket();
  if(globalOSCPageControl>0 && udpPacketSize){
    globalUDP.read(udpBuf, udpPacketSize);
    OSCMessage msg(udpBuf, udpPacketSize);
    if(msg.getAddress()=="/page"){
      byte p = msg.getInt(0);
      if(p==1 || p==2 || p==3 || p==4){
        changePage(p);
      }
    }
    
  }
  
}

void Net::loadEEPROM(){
  this->netMode = EEPROM.read(32);
  IP_Static = {EEPROM.read(34), EEPROM.read(35), EEPROM.read(36), EEPROM.read(37)};
  IP_Subnet = {EEPROM.read(38), EEPROM.read(39), EEPROM.read(40), EEPROM.read(41)};
  IP_Gateway = {EEPROM.read(42), EEPROM.read(43), EEPROM.read(44), EEPROM.read(45)};
  IP_Destination = {EEPROM.read(48), EEPROM.read(49), EEPROM.read(50), EEPROM.read(51)};

  EEPROM.get(46, IP_SelfPort);
  EEPROM.get(52, IP_DestinationPort);

  Serial.println("Network EEPROM Load OK");
}

byte Net::getIP_Static(byte i){
  return IP_Static[i];
}
byte Net::getIP_Subnet(byte i){
  return IP_Subnet[i];
}
byte Net::getIP_Gateway(byte i){
  return IP_Gateway[i];
}
byte Net::getIP_Destination(byte i){
  return IP_Destination[i];
}
byte Net::getMAC(byte i){
  return macAddress[i];
}
unsigned short Net::getIP_SelfPort(){
  return IP_SelfPort;
}
unsigned short Net::getIP_DestinationPort(){
  return IP_DestinationPort;
}
