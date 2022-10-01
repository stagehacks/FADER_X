boolean zeroFlag = false;
void danceLoop(){
  long mils = millis()-globalLastBoot;
  
  if(mils<2500){
    setFaderTarget(fader1.getChannel(), dancePos(mils-0));
    setFaderTarget(fader2.getChannel(), dancePos(mils-200));
    setFaderTarget(fader3.getChannel(), dancePos(mils-400));
    setFaderTarget(fader4.getChannel(), dancePos(mils-600));
    setFaderTarget(fader5.getChannel(), dancePos(mils-800));
    setFaderTarget(fader6.getChannel(), dancePos(mils-1000));
    setFaderTarget(fader7.getChannel(), dancePos(mils-1200));
    setFaderTarget(fader8.getChannel(), dancePos(mils-1400));
    
  }else if(mils>2500 && !zeroFlag){
    zeroFlag = true;
    for(byte i=0; i<255; i++){
      globalFaderTargets[i] = 0;
    }
    
  }else if(mils>2500){
    pauseAllFaders();
    globalMode = EEPROM.read(24);
    globalLastBoot = 0;
    unpauseAllFaders();
  }
}


int dancePos(int mod){
  int c = min(max(mod, 0), 1000)-400;
  return (sin(c/128.0)+1)*512;
}
