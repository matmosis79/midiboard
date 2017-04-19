#ifndef Button_h
#define Button_h

/**
    @file		Button.h
    Project		axefx.de MIDI Borad
 	@brief		Header-File for single class representing one Button
 	@version	1.0.3
    @author		Bastian Buehrig
 	@date		14/11/13
    license		GPL axefx.de - 2013
*/

// ==== Arduino-Standard-Lib
#include <Arduino.h>

class Button {
  public:
    // Class Variable


    // Public Attributes


    // Constructors
    Button();
    Button(byte btnPin, byte ledPin);

    // Destructors
    ~Button();

    // Public functions
    byte checkState();
    void select();
    void setLEDGroup(byte ledGroup[], byte ledGroupQty);
    byte getLedPin();


  private:
    // Private Attributes
    byte _actState;               // Actual Button-State:  LOW - unpressed    HIGH - pressed

    byte _btnPin;                 // Pin-Number for Buttons
    byte _ledPin;                 // Pin-Number for Button Status-LED
    byte _ledGroupQty;


    byte *_ledGroup;

    boolean status;
    // Private Functions


};



#endif




