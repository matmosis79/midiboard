/**
    @file		midi_board_ONE.ino
    Project		axefx.de MIDI Borad
 	@brief		MIDI-Board Main Program
 	@version	1.0.2
    @author		Bastian Buehrig
 	@date		14/11/13
    license		GPL axefx.de - 2013
*/


/****************************************************
 **
 **  Uses Arduino-MIDI-Library (v3.2)
 **  http://playground.arduino.cc/Main/MIDILibrary
 **  http://sourceforge.net/projects/arduinomidilib/
 **
 *****************************************************/

/**

    INCLUDES

 **/

// ==== Button-Class
#include "Button.h"

// ==== 3rd Party Libraries
#include "MIDI.h"
#include "TM1637Display.h"

// ============== SETUP ===============================

MIDI_CREATE_DEFAULT_INSTANCE();

#define CLK 9 //Set the CLK pin connection to the display
#define DIO 8 //Set the DIO pin connection to the display
TM1637Display display(CLK, DIO);  //set up the 4-Digit Display.
const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

/** MIDI-Channel-Number for sending the MIDI-Messages */
// MIDI-Channel for sending Commands
#define MIDI_CHANNEL 1

// ============== Button-Definitions: =================
//
Button buttons[] = {
  //    Button-Pin   LED-Pin
  Button(    53,       51 ),
  Button(    49,       47 ),
  Button(    45,       43 ),
  Button(    41,       39 ),
  Button(    37,       35 ),
  Button(    33,       31 ),
  Button(    29,       27 ),
  Button(    25,       23 ),
};
byte currBtnNo = 0;

#define BANK_DOWN 18
#define BANK_UP 19
short bank = 0; // (actual bank selected)
boolean bankTemp = false;

// =============== Incremento temporaneo di volume ===================
#define VOLUME_CC 11
byte precVolumeState = HIGH;
byte volumeCC = 7;
byte volemIncr = 127;

// =============== Defining LED-Groups ===================
//
byte groupPresets[] = {
  51, 47, 43, 39, 35, 31, 27, 23
};

// =============== Defining Functions ===================

void toDisplay(int num, bool bankChange = false, bool volumeUp = false) {
  uint8_t data[] = { 0, 0, 0, 0 };

  int n = num;
  int i = 3;
  while (n) { // loop till there's nothing left
    data[i--] = display.encodeDigit(n % 10); // assign the last digit
    n /= 10; // "right shift" the number
  }

  // se il numero è 0 deve essere visualizzato
  if (num == 0) data[3] = display.encodeDigit(0);

  // se minore di 10 inserimao un leading zero
  if (num < 10) data[2] = display.encodeDigit(0);

  // se è cambiato il bank visualizziamo un trattino come ultimo numero
  if (bankChange) data[3] = SEG_G;

  // se è impostato il volumeUp visualizziamo un simbolo come primo numero
  if (volumeUp) data[0] = SEG_A | SEG_B | SEG_F;

  display.setSegments(data);
}

void setLedBlink() {
  if (!bankTemp) return;

  digitalWrite(buttons[currBtnNo].getLedPin(), 1 - digitalRead(buttons[currBtnNo].getLedPin()));
  delay(100);
}

void delayedConnect() {
  // visualizziamoun un conto alla rovescia alla connessione MIDI...
  for (byte i = 6; i-- > 0; ) {
    toDisplay(i);
    delay(1000);
  }

  // Done!
  display.setSegments(SEG_DONE);
  delay(2000);
}

// ====
void setup() {
  // Setting LED-Groups to Buttons if needed
  buttons[0].setLEDGroup(groupPresets, sizeof(groupPresets) );
  buttons[1].setLEDGroup(groupPresets, sizeof(groupPresets) );
  buttons[2].setLEDGroup(groupPresets, sizeof(groupPresets) );
  buttons[3].setLEDGroup(groupPresets, sizeof(groupPresets) );
  buttons[4].setLEDGroup(groupPresets, sizeof(groupPresets) );
  buttons[5].setLEDGroup(groupPresets, sizeof(groupPresets) );
  buttons[6].setLEDGroup(groupPresets, sizeof(groupPresets) );
  buttons[7].setLEDGroup(groupPresets, sizeof(groupPresets) );

  pinMode(BANK_DOWN, INPUT_PULLUP);
  pinMode(BANK_UP, INPUT_PULLUP);
  pinMode(VOLUME_CC, INPUT_PULLUP);

  // DISPLAY (4 digits)
  // set the diplay to maximum brightness
  display.setBrightness(0x0a);

  // aspettiamo un po prima di connetterci al MIDI device da pilotare...
  delayedConnect();

  // MIDI-Initialising
  MIDI.begin();
  //  Serial.begin(9600);

  // per adesso di default accendiamo il primo button
  buttons[0].select();
  toDisplay(currBtnNo);

  MIDI.sendProgramChange(currBtnNo, MIDI_CHANNEL);
  //  Serial.print("MIDI PC: ");
  //  Serial.print(0);
  //  Serial.print(" on channel: ");
  //  Serial.println(MIDI_CHANNEL);
}

void loop() {
  // BANK SELECTOR start

  // check if the pushbutton is pressed.
  if (digitalRead (BANK_DOWN) == LOW) {
    delay(250);
    bank = bank - 10;
    if (bank < 0) {
      bank = 120;
    }

    toDisplay(bank + currBtnNo, true);
    bankTemp = true;
  }

  // check if the pushbutton is pressed.
  if (digitalRead (BANK_UP) == LOW) {
    delay(250);
    bank = bank + 10;
    if (bank > 120) {
      bank = 0;
    }

    toDisplay(bank + currBtnNo, true);
    bankTemp = true;
  }

  // VOLUME CONTROL CHANGE start
  if (digitalRead (VOLUME_CC) != precVolumeState) {
    if (digitalRead (VOLUME_CC) == LOW) {
      // premuto
      MIDI.sendControlChange(volumeCC, volemIncr, MIDI_CHANNEL);
      toDisplay(bank + currBtnNo, false, true);
    } else {
      // ricarichiamo il preset corrente. Questo annullerà la modifica
      // al parametro VOLUME (CC #7) di cui sopra
      MIDI.sendProgramChange(bank + currBtnNo, MIDI_CHANNEL);
      toDisplay(bank + currBtnNo);
    }

    precVolumeState = digitalRead (VOLUME_CC);
    delay(50);
  }

  setLedBlink();

  // Read Button-State und check for activation!
  for (byte btnNo = 0; btnNo < sizeof(buttons) / sizeof(Button); btnNo++) {
    byte ret = buttons[btnNo].checkState();

    if (ret == 1) {
      currBtnNo = btnNo;
      bankTemp = false;

      toDisplay(bank + currBtnNo);

      MIDI.sendProgramChange(bank + currBtnNo, MIDI_CHANNEL);
      //      Serial.print("MIDI PC: ");
      //      Serial.print((bank + btnNo));
      //      Serial.print(" on channel: ");
      //      Serial.println(MIDI_CHANNEL);
    }
  }

}

