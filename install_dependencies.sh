#!/bin/sh

# This file is only used by the Arduino_CI GitHub action to install
# the libraries some examples depend upon before compiling

curl -Lo ArduinoJoystickLibrary-2.0.7.tar.gz https://github.com/MHeironimus/ArduinoJoystickLibrary/archive/refs/tags/v2.0.7.tar.gz
tar zxvf ArduinoJoystickLibrary-2.0.7.tar.gz
rm -f ArduinoJoystickLibrary-2.0.7.tar.gz
