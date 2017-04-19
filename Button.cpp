/**
    @file		Button.cpp
    Project		axefx.de MIDI Borad
 	@brief		single class representing one Button
 	@version	1.0.3
    @author		Bastian Buehrig
 	@date		14/11/13
    license		GPL axefx.de - 2013
*/

#include "Button.h"



/**
   Standard-Constructor

*/
Button::Button() {

}



/**
   Constructor for Button-Initialization

   @param btnPin          Pin-Number where the switch is connected
   @param ledPin          Pin-Number where the LED is connected
*/
Button::Button(byte    btnPin,           /** Pin-Number where the switch is connected */
               byte    ledPin            /** Pin-Number where the LED is connected */
              ) {

  // Setting instance-variables
  _btnPin = btnPin;
  _ledPin = ledPin;

  // Initialise quantity of MIDI-Messages and LED-Groups
  _ledGroupQty = 0;


  // Set Pin-Mode für Switch and LED
  pinMode(_btnPin, INPUT_PULLUP);
  pinMode(_ledPin, OUTPUT);


  // Set initial-Button-State
  _actState = HIGH;

}



/**
   Standard-Destructor

*/
Button::~Button() {
}



/**
   This method will called, to check Button-State and will
   send a configured MIDI-Message. Depending on the
   Button-Configuration will send a PC or CC Message

   @return    void
*/
byte Button::checkState() {
  // Check if Button-State is different from the call before
  byte actState = digitalRead(_btnPin);
  byte ret = 0;

  if (_actState != actState) {
    // Button state was changed --> Do something!
    // Save aktcual Button State
    _actState = actState;
    delay(5);


    if (actState == LOW) {
      // Only do, if Button is pressed!
      //

      // First switch off all LEDs of the LED-Group!
      if (_ledGroupQty > 0 ) {
        for (byte i = 0; i < _ledGroupQty; i++) {
          digitalWrite(_ledGroup[i], LOW);
        }
      }

      digitalWrite(_ledPin, HIGH);
      ret = 1;
    }
  }

  return ret;
}

void Button::select() {
  // First switch off all LEDs of the LED-Group!
  if (_ledGroupQty > 0 ) {
    for (byte i = 0; i < _ledGroupQty; i++) {
      digitalWrite(_ledGroup[i], LOW);
    }
  }
  digitalWrite(_ledPin, HIGH);
}

/**
   This Method adds a full LED-Group to the actual Button
   for disable the LEDS at any change

   @param ledGroup      Array of all LED-Pins
   @param messagesQty   Size of the array to calculate quantity of the array-elements

*/
void Button::setLEDGroup(byte ledGroup[], byte ledGroupQty) {
  // Set Instance-variables
  _ledGroup = ledGroup;
  _ledGroupQty = ledGroupQty / sizeof(byte);
}

byte Button::getLedPin() {
  return _ledPin;
}

