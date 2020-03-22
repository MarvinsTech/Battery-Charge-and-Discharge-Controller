#include "arduino.h"
#include "HC595.h"

/*
 * This library was developed by Jacques Bellavance.
 */

HC595::HC595(int count, int latchPin, int clockPin, int dataPin) {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  MYchipCount = count;
  MYlatchPin = latchPin;
  MYclockPin = clockPin;
  MYdataPin = dataPin;
  MYlastPin = count * 8 - 1;
  reset();
}

void HC595::isLeastSignificantBitFirst() {
	MYway = LSBF;
}

void HC595::isMostSignificantBitFirst() {
	MYway = MSBF;
}

void HC595::reset() {
  for (int i = 0 ; i < 8 ; i++) MYarray[i] = 0;
  send();
}

void HC595::sendToChips() {
  digitalWrite(MYlatchPin, LOW);
  digitalWrite(MYdataPin, LOW);
  for (int i = MYchipCount-1 ; i >= 0 ; i--) {
	  for (int j = 7; j >= 0; j--) {
		  digitalWrite(MYclockPin, LOW);
		  if ((MYarray[i] & MYmask[j]) > 0) {
			  digitalWrite(MYdataPin, HIGH);
		  }
		  digitalWrite(MYclockPin, HIGH);
		  digitalWrite(MYdataPin, LOW);
	  }
  }
  digitalWrite(MYlatchPin, HIGH);
}

void HC595::reverseSendToChips() {
	digitalWrite(MYlatchPin, LOW);
	digitalWrite(MYdataPin, LOW);
	for (int i = 0; i < MYchipCount; i++) {
		for (int j = 0; j < 8; j++) {
			digitalWrite(MYclockPin, LOW);
			if ((MYarray[i] & MYmask[j]) > 0) {
				digitalWrite(MYdataPin, HIGH);
			}
			digitalWrite(MYclockPin, HIGH);
			digitalWrite(MYdataPin, LOW);
		}
	}
	digitalWrite(MYlatchPin, HIGH);
}

void HC595::send() {
  if (MYway == MSBF) sendToChips();
  else               reverseSendToChips();
}

void HC595::setPin(int pin, int action) {
  int chip = int(pin / 8);
  int pos = pin - (chip * 8);
  switch (action) {
    case OFF:{
      MYarray[chip] = MYarray[chip] & (255 - MYmask[pos]);
      break;  
    }
    case ON:{
      MYarray[chip] = MYarray[chip] | MYmask[pos];
      break;
    }
    case TOGGLE:{
      if ((MYarray[chip] & MYmask[pos]) == 0) 
        MYarray[chip] = MYarray[chip] | MYmask[pos];
      else
        MYarray[chip] = MYarray[chip] & (255 - MYmask[pos]);
      break;
    }
  }
  send();
}

void HC595::setPins(int from, int to, int action) {
  for (int i = from ; i <= to ; i++) setPin(i, action);
}

int HC595::getState(int pin) {
  int chip = int(pin / 8);
  int pos = chip - (pin * 8);
  int actualValue = MYarray[chip] & MYmask[pos];
  if (actualValue == 0) return 0; else return 1;
}

String HC595::toString() {
  String s = "";
  for (int i = (MYchipCount - 1) ; i >= 0; i--) {
    s += byteToString(MYarray[i]);
    s += "|";
  }
  return s;
}

String HC595::byteToString(int data) {
  String s = "";
  for (int i = 7 ; i >= 0; i--) {
    s += bitRead(data,i);
  }
  return s;
}

int HC595::lastPin() {
	return MYlastPin;
}
