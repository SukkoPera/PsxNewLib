/*******************************************************************************
 * This file is part of PsxNewLib.                                             *
 *                                                                             *
 * Copyright (C) 2019-2020 by SukkoPera <software@sukkology.net>               *
 *                                                                             *
 * PsxNewLib is free software: you can redistribute it and/or                  *
 * modify it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * PsxNewLib is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * You should have received a copy of the GNU General Public License           *
 * along with PsxNewLib. If not, see http://www.gnu.org/licenses.              *
 *******************************************************************************
 *
 * This sketch can send commands to a PSX Jogcon controller to move it's motor
 *
 * This example drives the controller through the hardware SPI port, so pins are
 * fixed and depend on the board/microcontroller being used. For instance, on an
 * Arduino Uno connections must be as follows:
 *
 * CMD: Pin 11
 * DATA: Pin 12
 * CLK: Pin 13
 *
 * Any pin can be used for ATTN, but please note that most 8-bit AVRs require
 * the HW SPI SS pin to be kept as an output for HW SPI to be in master mode, so
 * using that pin for ATTN is a natural choice. On the Uno this would be pin 10.
 *
 * It also works perfectly on OpenPSX2AmigaPadAdapter boards (as it's basically
 * a modified Uno).
 *
 */

#include <PsxControllerHwSpi.h>

const byte PIN_PS2_ATT = 10;
PsxControllerHwSpi<PIN_PS2_ATT> psx;

void setup() {
  Serial.begin(115200);
  while(!Serial){}
  Serial.println(F("Ready!"));
}

void loop() {
  static bool haveController = false;
  static uint8_t force = 8;
  
  if (!haveController) {
    if (psx.begin()) {
      Serial.println(F("Controller found!"));
      haveController = true;

      delay(300);
      
      if (!psx.enterConfigMode ()) {
        Serial.println (F("Cannot enter config mode"));
      } else {
        //must enable analog mode to use jogcon's paddle
        if (!psx.enableAnalogSticks ()) 
          Serial.println (F("Cannot enable analog sticks"));
        
        if (!psx.enableAnalogButtons ())
          Serial.println (F("Cannot enable analog buttons"));

        //must enable rumble to use the jogcon's motor
        if (!psx.enableRumble ())
          Serial.println (F("Cannot enable rumble"));
        
        if (!psx.exitConfigMode ())
          Serial.println (F("Cannot exit config mode"));
      }
      psx.read ();    // Make sure the protocol is up to date
    }
  } else {
    if(!psx.read()){
      haveController = false;
    } else if (psx.getProtocol () == PSPROTO_JOGCON) {// It's a jogcon!

      //reading the "raw" values
      uint8_t jogPosition = 0;
      uint8_t jogRevolutions = 0;
      JogconDirection jogDirection;
      JogconCommand cmdResult;

      //Checking if the jog state has changed or theres as command result.
      //Not required but just to not fill the serial output when controller is idle.
      if (psx.getJogconData(jogPosition, jogRevolutions, jogDirection, cmdResult) && (jogDirection != JOGCON_DIR_NONE || cmdResult != JOGCON_CMD_NONE)) {
        Serial.print(F("Position: "));
        Serial.print(jogPosition);
        Serial.print(F("\tRevolutions: "));
        Serial.print(jogRevolutions);

        if (jogDirection != JOGCON_DIR_NONE) {
          Serial.print(F("\tRotation: "));
          if (jogDirection == JOGCON_DIR_CCW)
            Serial.print(F("CCW"));
          else if (jogDirection == JOGCON_DIR_CW)
            Serial.print(F("CW"));
          else if (jogDirection == JOGCON_DIR_MAX)
            Serial.print(F("MAX"));
          else
            Serial.print(psx.getAnalogButton(PSAB_PAD_UP), HEX);
        }

        if (cmdResult != JOGCON_CMD_NONE) {
          Serial.print(F("\tCmdResult: "));
          if (cmdResult == JOGCON_CMD_DROP_REVOLUTIONS)
            Serial.print(F("DROP_REVOLUTIONS"));
          else if (cmdResult == JOGCON_CMD_NEW_START)
            Serial.print(F("NEW_START"));
          else
            Serial.print(psx.getAnalogButton(PSAB_PAD_UP), HEX);
        }
        Serial.println();
      } // end getJogconData()


      //buttons to decrement/increment the motor's force
      if (psx.buttonJustPressed (PSB_L2)) {
        force--;
        if (force > 15)
          force = 0;
        Serial.print(F("Motor force: "));
        Serial.println(force);
      } else if (psx.buttonJustPressed (PSB_R2)) { //increment the motor's force
        force++;
        if (force > 15)
          force = 15;
        Serial.print(F("Motor force: "));
        Serial.println(force);
      }


      //Send command no jogcon. Will be sent on next Read()
      JogconDirection newDirection = JOGCON_DIR_NONE;
      JogconCommand newCommand = JOGCON_CMD_NONE;

      //buttons to change the jogcon's mode

      //rotate to start position
      if (psx.buttonPressed (PSB_PAD_UP)) //PSB_TRIANGLE
        newDirection = JOGCON_DIR_START;
      //rotate CCW
      else if (psx.buttonPressed (PSB_PAD_LEFT)) //PSB_SQUARE
        newDirection = JOGCON_DIR_CCW;
      //rotate CW
      else if (psx.buttonPressed (PSB_PAD_RIGHT)) // PSB_CIRCLE
        newDirection = JOGCON_DIR_CW;

      //set new start position (and forget the ammount of revolutions)
      if (psx.buttonPressed (PSB_CROSS))
        newCommand = JOGCON_CMD_NEW_START;
      //forget the ammount of revolutions
      else if (psx.buttonPressed (PSB_TRIANGLE))
        newCommand = JOGCON_CMD_DROP_REVOLUTIONS;

      psx.setJogconMotorMode(newDirection, newCommand, force);

    } //end if PSPROTO_JOGCON
      
  }
  
}
