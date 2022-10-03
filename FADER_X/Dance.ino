boolean zeroFlag = false;
void danceLoop(){
  long mils = millis()-globalLastBoot;
  
  if(mils<600 && mils%5==0){
    zeroFlag = false;
    setFaderTarget(fader1.getChannel(), mils*3);
    setFaderTarget(fader2.getChannel(), mils*3-100);
    setFaderTarget(fader3.getChannel(), mils*3-200);
    setFaderTarget(fader4.getChannel(), mils*3-300);
    setFaderTarget(fader5.getChannel(), mils*3-400);
    setFaderTarget(fader6.getChannel(), mils*3-500);
    setFaderTarget(fader7.getChannel(), mils*3-600);
    setFaderTarget(fader8.getChannel(), mils*3-700);
    
  }else if(mils>600 && !zeroFlag){
    zeroFlag = true;
    for(byte i=0; i<255; i++){
      setFaderTarget(i, 0);
    }
    
  }else if(mils>600){
    pauseAllFaders();
    globalMode = EEPROM.read(24);
    globalLastBoot = 0;
    unpauseAllFaders();
  }
}


int dancePos(int mod){
  int c = min(max(mod, 0), 1000);
  return c;//(sin(c/128.0)+1)*512;
}
