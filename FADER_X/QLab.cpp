#include "QLab.h"
#include "Fader.h"
#include "Net.h"
#include "MIDI.h"
#include <EEPROM.h>

extern Net net;
extern Midi midi;
extern byte globalPage;
extern void channelUpdateAll();
extern void setFaderTarget(byte index, int value);
extern QLab qlab;
long lastHeartbeat = 0;

void parseMIDI(byte channel, byte fader, byte value) {
  qlab.faderTargetsMIDI[fader] = value*8;
  if(qlab.doMidiBehavior()){
    setFaderTarget(fader, qlab.faderTargetsMIDI[fader]);
  }
}

void QLab::setup(){
  usbMIDI.setHandleControlChange(parseMIDI);
  this->udp.begin(53001);
}

String jsonDataObject(String json){
  int startIndex = json.indexOf("\"data\":");
  int endIndex = json.indexOf(",\"", startIndex);
  return json.substring(startIndex + 7, endIndex);
}

int audioLevelToFader(float val){
  return pow(map(val, -60.0, 12.0, 0.0, 1.0), 1.3)*1023.0;
}
float faderToAudioLevel(float val){
  return map(pow(val/1023.0, 0.7692307), 0.0, 1.0, -60.0, 12.0);
}

char buf[Ethernet.mtu() - 20 - 8];
elapsedMillis sincePlaybackPositionChanged = 0;
void QLab::loop(){

  if(millis()-lastHeartbeat>10000){
    lastHeartbeat = millis();
    this->heartbeat();
  }

  int size = udp.parsePacket();
  if (0 < size && size <= sizeof(buf)) {
    udp.read(buf, size);
    OSCMessage msgIn(buf, size);
    parseOSC(msgIn);
  }else if(0 < size){
    Serial.println("packet too big");
  }

  usbMIDI.read(midi.listenChannel);

  if(sincePlaybackPositionChanged>200 && sincePlaybackPositionChanged<250){
    sincePlaybackPositionChanged = 250;
    
    OSCMessage msg("/cue/selected/sliderLevels");
    udp.beginPacket(net.IP_Destination, net.IP_DestinationPort);
    msg.writeUDP(&udp);
    udp.endPacket();

    OSCMessage msg1("/cue/selected/type");
    udp.beginPacket(net.IP_Destination, net.IP_DestinationPort);
    msg1.writeUDP(&udp);
    udp.endPacket();
  }
  
}


void QLab::parseOSC(OSCMessage msgIn){
  String oscAddr = msgIn.getAddress();

  if(oscAddr.indexOf("/dashboard")>0){
    
  }else if(msgIn.match("/reply/workspaces")){

    String json = msgIn.getString(0); 
    int s = json.indexOf("uniqueID\":\"");
    int e = json.indexOf("\",\"", s);
    String workspace = json.substring(s + 11, e);

    OSCMessage msg1("/workspace/"+workspace+"/connnect");
    msg1.addString(this->passcode);
    

    udp.beginPacket(net.IP_Destination, net.IP_DestinationPort);
    msg1.writeUDP(&udp);
    udp.endPacket();

    OSCMessage msg("/workspace/"+workspace+"/updates");
    msg.addInt(1);

    udp.beginPacket(net.IP_Destination, net.IP_DestinationPort);
    msg.writeUDP(&udp);
    udp.endPacket();

    OSCMessage msg2("/udpKeepAlive");
    msg2.addInt(1);

    udp.beginPacket(net.IP_Destination, net.IP_DestinationPort);
    msg2.writeUDP(&udp);
    udp.endPacket();


  }else if(msgIn.match("/update/workspace/*/cueList/*/playbackPosition")){
    sincePlaybackPositionChanged = 0;

  }else if(msgIn.match("/update/workspace/*/cue_id/*")){
    sincePlaybackPositionChanged = 100;

  }else if(msgIn.match("/reply/cue/selected/sliderLevels")){
    if(msgIn.getString(0).indexOf("\"status\":\"ok\"")==1){
      String levels = jsonDataObject(msgIn.getString(0));
      int c = 1;
      for(byte i=0; i<16; i++){
        float val = levels.substring(c, levels.indexOf(",", c)).toFloat();
        c = levels.indexOf(",", c) + 1;
        audioLevels[i] = val;
        this->faderTargetsOSC[i] = audioLevelToFader(audioLevels[i]);
        if(!this->doMidiBehavior()){
          setFaderTarget(i, this->faderTargetsOSC[i]);
        }
      }
    }else if(!this->doMidiBehavior()){
      for(byte i=0; i<255; i++){
        setFaderTarget(i, 0);
      }
    }
    
  }else if(msgIn.match("/reply/cue/selected/type") && this->autoMidi){
    String cueType = jsonDataObject(msgIn.getString(0));
    if(cueType=="\"Audio\"" || cueType=="\"Video\"" || cueType=="\"Fade\"" || cueType=="\"Mic\"" || cueType=="\"Camera\""){
      if(globalPage>1){
        globalPage-=2;
        this->changePage();
        channelUpdateAll();
      }
    }else if(globalPage<2){
      globalPage+=2;
      this->changePage();
      channelUpdateAll();
    }
  }
}

void QLab::touchEvent(int channel, Fader *fader){

  int position = fader->getPositionTrimmed();

  if(this->doMidiBehavior()){
    usbMIDI.sendControlChange(channel, position/8, midi.sendChannel);
    
  }else{
    OSCMessage msg("/cue/selected/level/0/"+String(channel));
    msg.addFloat(faderToAudioLevel(position));
    
    udp.beginPacket(net.IP_Destination, net.IP_DestinationPort);
    msg.writeUDP(&udp);
    udp.endPacket();
  }

}

void QLab::changePage(){
  if(doMidiBehavior()){
    for(byte i=0; i<128; i++){
      setFaderTarget(i, this->faderTargetsMIDI[i]);
    }
  }else{
    for(byte i=0; i<64; i++){
      setFaderTarget(i, this->faderTargetsOSC[i]);
    }
  }
}


void QLab::heartbeat(){
  OSCMessage msg("/workspaces");
  udp.beginPacket(net.IP_Destination, net.IP_DestinationPort);
  msg.writeUDP(&udp);
  udp.endPacket();
}

boolean QLab::doMidiBehavior(){
  return this->midiEnabled && (globalPage==2 || globalPage==3);
}
