boolean zeroFlag = false;
void danceLoop(){
  long mils = millis()-globalLastBoot;
  
  if(mils<100){
    zeroFlag = false;
    setFaderTarget(fader1.getChannel(), 1000);
    setFaderTarget(fader3.getChannel(), 1000);
    setFaderTarget(fader5.getChannel(), 1000);
    setFaderTarget(fader7.getChannel(), 1000);

  }else if(mils>300 && mils<350){
    setFaderTarget(fader2.getChannel(), 1000);
    setFaderTarget(fader4.getChannel(), 1000);
    setFaderTarget(fader6.getChannel(), 1000);
    setFaderTarget(fader8.getChannel(), 1000);
   
    
  }else if(mils>600 && mils<650){
    for(byte i=0; i<255; i++){
      setFaderTarget(i, 0);
    }
    
  }else if(!zeroFlag && mils>1400){
    pauseAllFaders();
    globalMode = EEPROM.read(24);
    globalLastBoot = 0;
    unpauseAllFaders();
    zeroFlag = true;
  }
}
