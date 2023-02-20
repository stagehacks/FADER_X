#include "Eos.h"
#include "Net.h"

extern Net net;
extern byte globalPage;
extern void pauseAllFaders();
extern void unpauseAllFaders();
extern void setFaderTarget(byte index, int value);

extern Fader fader1, fader2, fader3, fader4, fader5, fader6, fader7, fader8;
extern Encoder encoders[8];

long lastConnectionAttempt = 0;

void Eos::setup(){
  Serial.println("Eos setup");

  encoders[0].label("H ");
  encoders[1].label("S ");
  encoders[2].label("P ");
  encoders[3].label("T ");

}

void Eos::loop(){

  if(!connected && millis()-lastConnectionAttempt>5000){
    lastConnectionAttempt = millis();

    pauseAllFaders();
    int conn = this->eosClient.connect(net.IP_Destination, 3037);
    unpauseAllFaders();
    
    if(conn==1){
      OSCMessage msg("/eos/fader/"+String(this->faderBank)+"/config/"+this->faderCount);
      msg.writeSLIP(&eosClient);
      this->eosClient.flush();

      fader1.label(String(round(fader1.getPositionTrimmed()/10.23)).concat("%"));
      fader2.label(String(round(fader2.getPositionTrimmed()/10.23)).concat("%"));
      fader3.label(String(round(fader3.getPositionTrimmed()/10.23)).concat("%"));
      fader4.label(String(round(fader4.getPositionTrimmed()/10.23)).concat("%"));
      fader5.label(String(round(fader5.getPositionTrimmed()/10.23)).concat("%"));
      fader6.label(String(round(fader6.getPositionTrimmed()/10.23)).concat("%"));
      fader7.label(String(round(fader7.getPositionTrimmed()/10.23)).concat("%"));
      fader8.label(String(round(fader8.getPositionTrimmed()/10.23)).concat("%"));
    
      connected = true;
    }else{
      // Serial.print(millis());
      // Serial.println(" Could not connect to Eos");
      
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
    
    fader->label(String(round(fader->getPositionTrimmed()/10.23)).concat("%"));
  }
}

int hueValue = 0;
int satValue = 0;
int panValue = 0;
int panMin = -270;
int panMax = 270;

void Eos::knobEvent(byte channel, Encoder *encoder){
  byte i = encoder->realIndex;
  
  if(i==0){
    hueValue += encoder->direction*2;
    if(hueValue==361){
      hueValue = 0;
    }else if(hueValue==-1){
      hueValue = 360;
    }
    //hueValue = min(360, max(0, hueValue)); 
    encoder->label(String("H:").concat(hueValue));

  }else if(i==1){
    satValue += encoder->direction;
    satValue = min(100, max(0, satValue)); 
    encoder->label(String("S:").concat(satValue));
    
  }else if(i==2){
    OSCMessage msg("/eos/wheel/pan");
    msg.addFloat(encoder->direction*4);
    msg.writeSLIP(&eosClient);
    this->eosClient.flush();
    
  }else if(i==3){
    OSCMessage msg("/eos/wheel/tilt");
    msg.addFloat(encoder->direction*4);
    msg.writeSLIP(&eosClient);
    this->eosClient.flush();
    
  }

  if(i==0 || i==1){
    OSCMessage msg("/eos/color/hs");
    msg.addFloat(hueValue);
    msg.addFloat(satValue);
    msg.writeSLIP(&eosClient);
    this->eosClient.flush();
    
  }
  
}

void Eos::motorEvent(int channel, Fader *fader){
  fader->label(String(round(fader->getPositionTrimmed()/10.23)).concat("%"));
}



void Eos::processOSC(OSCMessage msg){

  if(msg.match("/eos/fader/"+String(this->faderBank)+"/*")){
    byte faderNum = msg.getAddress().substring(13).toInt();
    setFaderTarget(faderNum, msg.getFloat(0)*1023.0);
    //globalFaderTargets[faderNum] = msg.getFloat(0)*1023.0;
    
  }else if(msg.match("/eos/out/pantilt")){
    encoders[2].label(String("P:").concat((int)msg.getFloat(4)));
    encoders[3].label(String("T:").concat((int)msg.getFloat(5)));
    
  }
}
