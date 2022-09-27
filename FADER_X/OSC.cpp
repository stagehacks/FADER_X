#include "OSC.h"
#include <QNEthernet.h>
using namespace qindesign::network;

OSCMessage::OSCMessage(char* buf, int length) {
  int i=0;
  while(buf[i] != 0){
    this->addr+=buf[i];
    i++;
  }

  while(buf[i] == 0){
    i++;
  }
  i++;

  int parameterCount = 0;
  while(buf[i] != 0){
    paramTypes[parameterCount] = buf[i];
    parameterCount++;
    i++;
  }

  i = i + (4 - i%4);

  for(byte j = 0; j<parameterCount; j++){
    if(paramTypes[j]=='i' || paramTypes[j]=='f'){
      blobs[j].b[0] = buf[i+3];
      blobs[j].b[1] = buf[i+2];
      blobs[j].b[2] = buf[i+1];
      blobs[j].b[3] = buf[i+0];
      i+=4;

    }else if(paramTypes[j]=='s'){
      char strBuf[1024];
      int l = 0;
      while(buf[i]!=0){
        strBuf[l] = buf[i];
        i++;
        l++;
      }
      
      int mem = malloc(l);
      memcpy(mem, strBuf, l);
      
      blobs[j].s[0] = mem;
      blobs[j].s[1] = l;
     
    }
    
  }

}
OSCMessage::OSCMessage(String address){
  this->addr = address;
}


// Match

boolean OSCMessage::match(String test){
  String addr = getAddress();
  unsigned int s = 0;
  unsigned int e = test.indexOf("/", s);
  unsigned int t = 0;
  unsigned int f = addr.indexOf("/", s);
  test+="/";
  addr+="/";
  
  while(e<test.length()){
    String testPattern = test.substring(s, e);
    String addrPattern = addr.substring(t, f);
    
    if(!testPattern.equals(addrPattern) && testPattern!="*"){
      return false;
    }
    
    s = e+1;
    t = f+1;
    e = test.indexOf("/", s);
    f = addr.indexOf("/", t);
  }
 
  return true;
}

String OSCMessage::addressPart(byte part){
  String addr = getAddress();
  unsigned int s = 0;
  byte i = 0;
  while(i<=part){
    s = addr.indexOf("/", s)+1;
    i++;
  }  
  return addr.substring(s, addr.indexOf("/", s));
}


// Write

void OSCMessage::writeSLIP(EthernetClient *client){
  client->write(0xC0);

  // Address
  client->print(this->addr);
  nullPad(client, 4-this->addr.length()%4);

  // Parameter Types
  client->write(",");
  for(byte i=0; i<this->parameterCount; i++){
    client->write(this->paramTypes[i]);
  }
  nullPad(client, 4-(1+this->parameterCount)%4);
  
  for(byte i=0; i<this->parameterCount; i++){
    char type = this->paramTypes[i];
    if(type=='f' || type=='i'){
      this->byteSLIP(client, blobs[i].b[3]);
      this->byteSLIP(client, blobs[i].b[2]);
      this->byteSLIP(client, blobs[i].b[1]);
      this->byteSLIP(client, blobs[i].b[0]);
    }
  }

  client->write(0xC0);
}

void OSCMessage::writeUDP(EthernetUDP *udp){

  // Address
  udp->print(this->addr);
  nullPadUDP(udp, 4-this->addr.length()%4);

  // Parameter Types
  udp->write(",");
  for(byte i=0; i<this->parameterCount; i++){
    udp->write(this->paramTypes[i]);
  }
  nullPadUDP(udp, 4-(1+this->parameterCount)%4);

  for(byte i=0; i<this->parameterCount; i++){
    char type = this->paramTypes[i];
    if(type=='f' || type=='i'){
      udp->write(blobs[i].b[3]);
      udp->write(blobs[i].b[2]);
      udp->write(blobs[i].b[1]);
      udp->write(blobs[i].b[0]);
    }else if(type=='s'){
      udp->print(this->paramStrings[i]);
      nullPadUDP(udp, 4-paramStrings[i].length()%4);
    }
  }
  
}

// Pad

void OSCMessage::nullPad(EthernetClient *client, byte count){
  while(count>0){
    client->write((byte)0);
    count--;
  }
}
void OSCMessage::nullPadUDP(EthernetUDP *udp, byte count){
  while(count>0){
    udp->write((byte)0);
    count--;
  }
}

void OSCMessage::byteSLIP(EthernetClient *client, unsigned char b){
  if(b==192){
    client->write(219);
    client->write(220);
  }else if(b==219){
    client->write(219);
    client->write(221);
  }else{
    client->write(b);
  }
}

// Add

void OSCMessage::addFloat(float p){
  this->paramTypes[this->parameterCount] = 'f';
  this->blobs[this->parameterCount].f = p;
  this->parameterCount++;
}
void OSCMessage::addInt(int p){
  this->paramTypes[this->parameterCount] = 'i';
  this->blobs[this->parameterCount].i = p;
  this->parameterCount++;
}
void OSCMessage::addString(String p){
  this->paramTypes[this->parameterCount] = 's';
  this->paramStrings[this->parameterCount] = p;
  this->parameterCount++;
}

// Get

String OSCMessage::getAddress(){
  return String(addr);
}

int OSCMessage::getInt(byte pos){
  if(paramTypes[pos]!='i'){
    return 0;
  }
  return blobs[pos].i;
}

float OSCMessage::getFloat(byte pos){
  if(paramTypes[pos]!='f'){
    return 0;
  }
  return blobs[pos].f;
}

String OSCMessage::getString(byte pos){
  if(paramTypes[pos]!='s'){
    return "0";
  }
  char strBuf[blobs[pos].s[1]];
  memcpy(strBuf, blobs[pos].s[0], blobs[pos].s[1]);
  return String(strBuf);
}
