boolean zeroFlag = false;
void danceLoop(){
  if(globalClock<2500){
    setFaderTarget(fader1.getChannel(), dancePos(0));
    setFaderTarget(fader2.getChannel(), dancePos(200));
    setFaderTarget(fader3.getChannel(), dancePos(400));
    setFaderTarget(fader4.getChannel(), dancePos(600));
    setFaderTarget(fader5.getChannel(), dancePos(800));
    setFaderTarget(fader6.getChannel(), dancePos(1000));
    setFaderTarget(fader7.getChannel(), dancePos(1200));
    setFaderTarget(fader8.getChannel(), dancePos(1400));
    
  }else if(globalClock>2500 && !zeroFlag){
    zeroFlag = true;
    for(byte i=0; i<255; i++){
      globalFaderTargets[i] = 0;
    }
    
  }else if(globalClock>2500){
    pauseAllFaders();
    globalMode = EEPROM.read(24);
    globalClock = 0;
    unpauseAllFaders();
  }
}


int dancePos(int mod){
  int c = min(max((int)globalClock-mod, 0), 1000)-400;
  return (sin(c/128.0)+1)*512;
}
