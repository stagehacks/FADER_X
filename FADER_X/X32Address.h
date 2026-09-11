#ifndef X32_ADDRESS_H
#define X32_ADDRESS_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

inline uint8_t x32FaderTargetLimit(bool xAir, uint8_t target){
  if(target == 1) return xAir ? 16 : 32;
  if(target == 2) return xAir ? 4 : 8;
  if(target == 3) return xAir ? 6 : 16;
  if(target == 4) return xAir ? 1 : 8;
  if(target == 5) return xAir ? 4 : 8;
  if(target == 6) return xAir ? 0 : 8;
  return 0;
}

inline bool x32FaderTargetSupported(bool xAir, uint8_t target, uint8_t channel){
  return channel > 0 && channel <= x32FaderTargetLimit(xAir, target);
}

inline bool x32FaderAddress(bool xAir, uint8_t target, uint8_t channel, char* address, size_t size){
  if(!address || !size || !x32FaderTargetSupported(xAir, target, channel)){
    return false;
  }

  int written = -1;
  if(target == 1){
    written = snprintf(address, size, "/ch/%02u/mix/fader", channel);
  }else if(target == 2){
    written = snprintf(address, size, "/dca/%u/fader", channel);
  }else if(target == 3){
    written = snprintf(address, size, "/bus/%02u/mix/fader", channel);
  }else if(target == 4 && xAir){
    written = snprintf(address, size, "/rtn/aux/mix/fader");
  }else if(target == 4 && !xAir){
    written = snprintf(address, size, "/auxin/%02u/mix/fader", channel);
  }else if(target == 5 && xAir){
    written = snprintf(address, size, "/rtn/%u/mix/fader", channel);
  }else if(target == 5 && !xAir){
    written = snprintf(address, size, "/fxrtn/%02u/mix/fader", channel);
  }else if(target == 6 && !xAir){
    written = snprintf(address, size, "/mtx/%02u/mix/fader", channel);
  }

  return written > 0 && static_cast<size_t>(written) < size;
}

#endif
