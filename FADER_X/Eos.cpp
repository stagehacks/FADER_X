#include "Eos.h"
#include "Net.h"

extern Net net;
extern byte globalPage;
extern void pauseAllFaders();
extern void unpauseAllFaders();

long lastConnectionAttempt = 0;

void Eos::setup(){
  Serial.println("Eos setup");

}

void Eos::loop(){

  if(!connected && millis()-lastConnectionAttempt>5000){
    lastConnectionAttempt = millis();
    pauseAllFaders();
    
    delay(100);
    int conn = this->eosClient.connect(net.IP_Destination, 3037);
    delay(100);
    
    if(conn==1){
      OSCMessage msg("/eos/fader/"+String(this->faderBank)+"/config/"+this->faderCount);
      msg.writeSLIP(&eosClient);
      this->eosClient.flush();
    
      connected = true;
      unpauseAllFaders();
    }else{
      Serial.println("Could not connect to Eos");
    }
    
  }
  
  char buf[128];
  int i = 0;
  while(this->eosClient.available()){
    byte b = this->eosClient.read();
    
    if(b==0xC0){
      if(i>0){
        OSCMessage msg(buf, i);
        processOSC(msg);
      }
      i=0;
    }else{
      buf[i] = b;
      i++;
    }
  }
}

void Eos::touchEvent(int channel, Fader *fader){ 
  if(connected){
    OSCMessage msg2("/eos/fader/"+String(this->faderBank)+"/"+channel);
    msg2.addFloat((float)(fader->getPositionTrimmed()/1023.0));
    msg2.writeSLIP(&eosClient);
    this->eosClient.flush();
  }
}

void Eos::processOSC(OSCMessage msg){

  if(msg.match("/eos/fader/"+String(this->faderBank)+"/*")){
    byte faderNum = msg.getAddress().substring(13).toInt();
    globalFaderTargets[faderNum] = msg.getFloat(0)*1023.0;
  }
}
