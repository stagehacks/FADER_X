#include <Arduino.h>
#include "Fader.h"

class Midi {
  private:

  public:
    void setup();
    void loop();

    void touchEvent(int channel, Fader *fader);
    void knobEvent(byte channel, Encoder *encoder);
    void motorEvent(int channel, Fader *fader);

    byte listenChannel = 1;
    byte sendChannel = 1;
};
