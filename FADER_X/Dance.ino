void danceLoop(){
  if(globalClock<2000){
    setFaderTarget(fader1.getChannel(), (sin(globalClock/256.0)+1)*512);
    setFaderTarget(fader2.getChannel(), (sin((globalClock-200)/256.0)+1)*512);
    setFaderTarget(fader3.getChannel(), (sin((globalClock-400)/256.0)+1)*512);
    setFaderTarget(fader4.getChannel(), (sin((globalClock-600)/256.0)+1)*512);
    setFaderTarget(fader5.getChannel(), (sin((globalClock-800)/256.0)+1)*512);
    setFaderTarget(fader6.getChannel(), (sin((globalClock-1000)/256.0)+1)*512);
    setFaderTarget(fader7.getChannel(), (sin((globalClock-1200)/256.0)+1)*512);
    setFaderTarget(fader8.getChannel(), (sin((globalClock-1400)/256.0)+1)*512);
    
  }else if(globalClock>2000 && globalClock<2100){
    for(byte i=0; i<255; i++){
      globalFaderTargets[i] = 0;
    }
    
  }else if(globalClock>2100){
    pauseAllFaders();
    globalMode = EEPROM.read(24);
    globalClock = -5000;
    unpauseAllFaders();
  }
}
