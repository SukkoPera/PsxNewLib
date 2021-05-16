#include <PsxControllerHwSpi.h>
#include <PsxNewLib.h>

PsxControllerHwSpi<4> psxCtrl;

boolean connected = false;
boolean axisConfigPossible = true;
boolean axisSticksEnabled = false;

void setup() {
  psxCtrl.begin();
}

void loop() {
  if(connected && axisConfigPossible && !axisSticksEnabled){
    Serial.println("Attempting to enable axis sticks & rumble!");
    psxCtrl.setRumble(false, 0x00);
    if(psxCtrl.enterConfigMode() && psxCtrl.enableAnalogSticks(true, true)){
      psxCtrl.enableRumble();
      psxCtrl.exitConfigMode();
      Serial.println("Axis sticks enabled! Rumble enabled!");
      axisSticksEnabled = true;
    }
    else{
      Serial.println("Failed to enable axis sticks!");
      axisConfigPossible = false; // don't retry
    }
  }
  // put your main code here, to run repeatedly:
  if(psxCtrl.read()) {
    if(!connected && Serial) Serial.println("Found controller!");
    connected = true;
    if(axisSticksEnabled) {
      if(psxCtrl.buttonChanged(PSB_CROSS)) {
        if(psxCtrl.buttonPressed(PSB_CROSS)) {
          Serial.println("Rumbling...");
          psxCtrl.setRumble(true, 0xFF);
        }
        else {
          Serial.println("Ending rumble.");
          psxCtrl.setRumble(false, 0x00);
        }
      }
    }
  }
  else {
    if(connected && Serial) Serial.println("Lost controller!");
    connected = false;
    axisSticksEnabled = false;
    axisConfigPossible = true;
  }
}
