    /////////////////////////
   //       ChargeOS      //
  //     Version: 1.30   //
 //   By MarvinsTech    //
/////////////////////////

//https://github.com/MarvinsTech/Battery-charge-and-discharge-controller

#include "bitmaps.h"
#include "pins.h"
#include "pitches.h"
#include <SPI.h>
#include <Wire.h>
#include "AHT10.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "HC595.h"

#define OLED_RESET 13
#define DisplayAdress 0x3C
Adafruit_SSD1306 display(OLED_RESET);

uint8_t readStatus = 0;

AHT10 myAHT10(AHT10_ADDRESS_0X38);

const int chipCount = 3;  // Number of serialy connected 74HC595 (8 maximum)
HC595 Shiftregisters(chipCount, STCP, SHCP, DS);

int lastPin = Shiftregisters.lastPin();

////////////////////Values////////////////////////////

float CellVoltage1;
float CellVoltage2;
float CellVoltage3;
float CellVoltage4;
float CellVoltage5;
float CellVoltage6;

float CalCell1Factor = 1005.0;
float CalCell2Factor = 1005.0;
float CalCell3Factor = 1005.0;
float CalCell4Factor = 1005.0;
float CalCell5Factor = 1005.0;
float CalCell6Factor = 1005.0;

float ResistorTemperature;

int FanSpeed;
int TempMin = 20; //The temperature to start the fan
int TempMax = 55; //The maximum temperature when fan is at 100%
int AlertTemp = 70;

long Time;
long Time2;

int RenewalVariable = 0;

float Version = 1.25;

int ScrollValue1 = 0;


int DisplayFanSpeed;

int ChargeTmp1 = 0;
int ChargeTmp2 = 0;
int ChargeTmp3 = 0;
int ChargeTmp4 = 0;
int ChargeTmp5 = 0;
int ChargeTmp6 = 0;

bool buttonStateR = 0;
bool buttonStateU = 0;
bool buttonStateD = 0;
bool buttonStateM = 0;

/////////////////////////////Shiftregister////////////////////////////////

bool Load1 = 0;
bool Load2 = 0;
bool Load3 = 0;
bool Load4 = 0;
bool Load5 = 0;
bool Load6 = 0;

bool Charge1 = 0;
bool Charge2 = 0;
bool Charge3 = 0;
bool Charge4 = 0;
bool Charge5 = 0;
bool Charge6 = 0;

bool LED_Green1 = 1;
bool LED_Green2 = 1;
bool LED_Green3 = 1;

bool LED_Red1 = 1;
bool LED_Red2 = 1;
bool LED_Red3 = 1;

bool LED_Blue1 = 1;
bool LED_Blue2 = 1;
bool LED_Blue3 = 1;

/* 7-Load1
   6-Load2
   5-Load3
   4-Load4
   3-Load5
   2-Load6
   1-Charge1  (One)
   0-Charge2  (One)
   15-Charge3  (Two)
   14-Charge4 (Two)
   13-Charge5 (Three)
   12-Charge6 (Three)
   11-LED_Green1  (One)
   10-LED_Red1  (One)
   9-LED_Blue1  (One)
   8-LED_Green2  (Two)
   23-LED_Red2  (Two)
   22-LED_Blue2  (Two)
   21-LED_Green3  (Three)
   20-LED_Red3  (Three)
   19-LED_Blue3  (Three)
   18-Nothing
   17-Nothing
   16-Nothing
*/

////////////////////////////////////1//2//3//4//5//6//7//8//9/10/11/12/13/14/15/16/17/18/19/20/21/22/23/24
unsigned ShiftregisterBinOut[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Init();
  Time = millis();
  Time2 = millis();
}

void loop() {
  while (!buttonStateR && !buttonStateU && !buttonStateD && !buttonStateM) { //For testing
    ReadButtons();
  if (Time2 + 700 <= millis()) {
    Time2 = millis();
    PrintTemperatureSerial();
    PrintVoltageSerial();
  }

  if (Time + 2250 <= millis()) {
    UpdateFanSpeed();
    MeasureTemperatre();
    MeasureVoltage();

    if (ScrollValue1 == 0) {
      Adafruit_SSD1306 display(OLED_RESET);
      display.begin(SSD1306_SWITCHCAPVCC, DisplayAdress);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print(F("Temp: ")); display.print(ResistorTemperature); display.print(F("C"));
      display.setCursor(0, 15);
      display.print(F("Fan speed: ")); display.print(DisplayFanSpeed); display.print(F("%"));
      display.display();
      ScrollValue1 = 1;
    } else if (ScrollValue1 == 1) {

      Adafruit_SSD1306 display(OLED_RESET);
      display.begin(SSD1306_SWITCHCAPVCC, DisplayAdress);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      if (CellVoltage1 >= 4.60) {
        display.setCursor(0, 0);
        display.print(F("Cell1: ")); display.print(F("0.00V"));
      } else {
        display.setCursor(0, 0);
        display.print(F("Cell1: ")); display.print(CellVoltage1); display.print(F("V"));
      }

      if (CellVoltage2 >= 4.60) {
        display.setCursor(0, 15);
        display.print(F("Cell2: ")); display.print(F("0.00V"));
      } else {
        display.setCursor(0, 15);
        display.print(F("Cell2: ")); display.print(CellVoltage2); display.print(F("V"));
      }
      display.display();
      ScrollValue1 = 2;
    } else if (ScrollValue1 == 2) {

      Adafruit_SSD1306 display(OLED_RESET);
      display.begin(SSD1306_SWITCHCAPVCC, DisplayAdress);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      if (CellVoltage3 >= 4.60) {
        display.setCursor(0, 0);
        display.print(F("Cell3: ")); display.print(F("0.00V"));
      } else {
        display.setCursor(0, 0);
        display.print(F("Cell3: ")); display.print(CellVoltage3); display.print(F("V"));
      }

      if (CellVoltage4 >= 4.60) {
        display.setCursor(0, 15);
        display.print(F("Cell4: ")); display.print(F("0.00V"));
      } else {
        display.setCursor(0, 15);
        display.print(F("Cell4: ")); display.print(CellVoltage4); display.print(F("V"));
      }
      display.display();
      ScrollValue1 = 3;

    } else if (ScrollValue1 == 3) {

      Adafruit_SSD1306 display(OLED_RESET);
      display.begin(SSD1306_SWITCHCAPVCC, DisplayAdress);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      if (CellVoltage5 >= 4.60) {
        display.setCursor(0, 0);
        display.print(F("Cell5: ")); display.print(F("0.00V"));
      } else {
        display.setCursor(0, 0);
        display.print(F("Cell5: ")); display.print(CellVoltage5); display.print(F("V"));
      }

      if (CellVoltage6 >= 4.60) {
        display.setCursor(0, 15);
        display.print(F("Cell6: ")); display.print(F("0.00V"));
      } else {
        display.setCursor(0, 15);
        display.print(F("Cell6: ")); display.print(CellVoltage6); display.print(F("V"));
      }
      display.display();
      ScrollValue1 = 0;
    }
    Time = millis();
  }

  if ((CellVoltage1 > 0.00) && (Load1 == 0) && (CellVoltage1 <= 4.25) && (ChargeTmp1 == 0)) { ////////Battery charging////////////
    Charge1 = 1;

    LED_Red1 = 0;
    LED_Green1 = 1;
    LED_Blue1 = 1;
    ChargeTmp1 = 1;
  }

  if ((CellVoltage2 > 0.00) && (Load2 == 0) && (CellVoltage2 <= 4.25) && (ChargeTmp2 == 0)) {
    Charge2 = 1;

    LED_Red1 = 0;
    LED_Green1 = 1;
    LED_Blue1 = 1;

    ChargeTmp2 = 1;
  }

  if ((CellVoltage3 > 0.00) && (Load3 == 0) && (CellVoltage3 <= 4.25) && (ChargeTmp3 == 0)) {
    Charge3 = 1;

    LED_Red2 = 0;
    LED_Green2 = 1;
    LED_Blue2 = 1;
    ChargeTmp3 = 1;
  }

  if ((CellVoltage4 > 0.00) && (Load4 == 0) && (CellVoltage4 <= 4.25) && (ChargeTmp4 == 0)) {
    Charge4 = 1;

    LED_Red2 = 0;
    LED_Green2 = 1;
    LED_Blue2 = 1;

    ChargeTmp4 = 1;
  }

  if ((CellVoltage5 > 0.00) && (Load5 == 0) && (CellVoltage5 <= 4.25) && (ChargeTmp5 == 0)) {
    Charge5 = 1;

    LED_Red3 = 0;
    LED_Green3 = 1;
    LED_Blue3 = 1;

    ChargeTmp5 = 1;
  }

  if ((CellVoltage6 > 0.00) && (Load6 == 0) && (CellVoltage6 <= 4.25) && (ChargeTmp6 == 0)) {
    Charge6 = 1;

    LED_Red3 = 0;
    LED_Green3 = 1;
    LED_Blue3 = 1;

    ChargeTmp6 = 1;
  }


  if ((CellVoltage1 >= 4.20) && (CellVoltage1 <= 4.40)) { ////////Battery full////////////
    Charge1 = 0;

    LED_Red1 = 1;
    LED_Green1 = 0;
    LED_Blue1 = 1;

    if (ChargeTmp1 == 1) {
      InformationTone();
      ChargeTmp1 = 2;
    }
  }

  if ((CellVoltage2 >= 4.20) && (CellVoltage2 <= 4.40)) {
    Charge2 = 0;

    LED_Red1 = 1;
    LED_Green1 = 0;
    LED_Blue1 = 1;

    if (ChargeTmp2 == 1) {
      InformationTone();
      ChargeTmp2 = 2;
    }
  }

  if ((CellVoltage3 >= 4.20) && (CellVoltage3 <= 4.40)) {
    Charge3 = 0;

    LED_Red2 = 1;
    LED_Green2 = 0;
    LED_Blue2 = 1;

    if (ChargeTmp3 == 1) {
      InformationTone();
      ChargeTmp3 = 2;
    }
  }

  if ((CellVoltage4 >= 4.20) && (CellVoltage4 <= 4.40)) {
    Charge4 = 0;

    LED_Red2 = 1;
    LED_Green2 = 0;
    LED_Blue2 = 1;

    if (ChargeTmp4 == 1) {
      InformationTone();
      ChargeTmp4 = 2;
    }
  }

  if ((CellVoltage5 >= 4.20) && (CellVoltage5 <= 4.40)) {
    Charge5 = 0;

    LED_Red3 = 1;
    LED_Green3 = 0;
    LED_Blue3 = 1;

    if (ChargeTmp5 == 1) {
      InformationTone();
      ChargeTmp5 = 2;
    }
  }

  if ((CellVoltage6 >= 4.20) && (CellVoltage6 <= 4.40)) {
    Charge6 = 0;

    LED_Red3 = 1;
    LED_Green3 = 0;
    LED_Blue3 = 1;

    if (ChargeTmp6 == 1) {
      InformationTone();
      ChargeTmp6 = 2;
    }
  }


  if (CellVoltage1 >= 4.50) { ////////Battery not connected////////////
    Charge1 = 0;

    LED_Red1 = 1;
    LED_Green1 = 1;
    LED_Blue1 = 0;
    ChargeTmp1 == 0;
  }

  if (CellVoltage2 >= 4.5) {
    Charge2 = 0;

    LED_Red1 = 1;
    LED_Green1 = 1;
    LED_Blue1 = 0;

    ChargeTmp2 == 0;
  }

  if (CellVoltage3 >= 4.5) {
    Charge3 = 0;

    LED_Red2 = 1;
    LED_Green2 = 1;
    LED_Blue2 = 0;

    ChargeTmp3 == 0;
  }

  if (CellVoltage4 >= 4.5) {
    Charge4 = 0;

    LED_Red2 = 1;
    LED_Green2 = 1;
    LED_Blue2 = 0;
    
    ChargeTmp4 == 0;
  }

  if (CellVoltage5 >= 4.5) {
    Charge5 = 0;

    LED_Red3 = 1;
    LED_Green3 = 1;
    LED_Blue3 = 0;

    ChargeTmp5 == 0;
  }

  if (CellVoltage6 >= 4.5) {
    Charge6 = 0;

    LED_Red3 = 1;
    LED_Green3 = 1;
    LED_Blue3 = 0;

    ChargeTmp6 == 0;
  }


  while (ResistorTemperature >= AlertTemp) {
    WarningTone();
    Load1 = 0;
    Load2 = 0;
    Load3 = 0;
    Load4 = 0;
    Load5 = 0;
    Load6 = 0;

    LED_Red1 = 0;
    LED_Green1 = 1;
    LED_Blue1 = 1;

    LED_Red2 = 0;
    LED_Green2 = 1;
    LED_Blue2 = 1;

    LED_Red3 = 0;
    LED_Green3 = 1;
    LED_Blue3 = 1;
    UpdateShiftregister();
    delay(250);
  }

  UpdateShiftregister();
  }
  Serial.println("TEST");
  delay(1000);
  ReadButtons();
}

void MeasureVoltage() {

  if (Load1 == 1) {
    Load1 = 0;
    UpdateShiftregister();
    CellVoltage1 = analogRead(CELL1);
    CellVoltage1 = CellVoltage1 * (5.0 / CalCell1Factor);
    Load1 = 1;
  } else if (Charge1 == 1) {
    Charge1 = 0;
    UpdateShiftregister();
    CellVoltage1 = analogRead(CELL1);
    CellVoltage1 = CellVoltage1 * (5.0 / CalCell1Factor);
    Charge1 = 1;
  } else {
    CellVoltage1 = analogRead(CELL1);
    CellVoltage1 = CellVoltage1 * (5.0 / CalCell1Factor);
  }

  if (Load2 == 1) {
    Load2 = 0;
    UpdateShiftregister();
    CellVoltage2 = analogRead(CELL2);
    CellVoltage2 = CellVoltage2 * (5.0 / CalCell2Factor);
    Load2 = 1;
  } else if (Charge2 == 1) {
    Charge2 = 0;
    UpdateShiftregister();
    CellVoltage2 = analogRead(CELL2);
    CellVoltage2 = CellVoltage2 * (5.0 / CalCell2Factor);
    Charge2 = 1;
  } else {
    CellVoltage2 = analogRead(CELL2);
    CellVoltage2 = CellVoltage2 * (5.0 / CalCell2Factor);
  }

  if (Load3 == 1) {
    Load3 = 0;
    UpdateShiftregister();
    CellVoltage3 = analogRead(CELL3);
    CellVoltage3 = CellVoltage3 * (5.0 / CalCell3Factor);
    Load3 = 1;
  } else if (Charge3 == 1) {
    Charge3 = 0;
    UpdateShiftregister();
    CellVoltage3 = analogRead(CELL3);
    CellVoltage3 = CellVoltage3 * (5.0 / CalCell3Factor);
    Charge3 = 1;
  } else {
    CellVoltage3 = analogRead(CELL3);
    CellVoltage3 = CellVoltage3 * (5.0 / CalCell3Factor);
  }

  if (Load4 == 1) {
    Load4 = 0;
    UpdateShiftregister();
    CellVoltage4 = analogRead(CELL4);
    CellVoltage4 = CellVoltage4 * (5.0 / CalCell4Factor);
    Load4 = 1;
  } else if (Charge4 == 1) {
    Charge4 = 0;
    UpdateShiftregister();
    CellVoltage4 = analogRead(CELL4);
    CellVoltage4 = CellVoltage4 * (5.0 / CalCell4Factor);
    Charge4 = 1;
  } else {
    CellVoltage4 = analogRead(CELL4);
    CellVoltage4 = CellVoltage4 * (5.0 / CalCell4Factor);
  }

  if (Load5 == 1) {
    Load5 = 0;
    UpdateShiftregister();
    CellVoltage5 = analogRead(CELL5);
    CellVoltage5 = CellVoltage5 * (5.0 / CalCell5Factor);
    Load5 = 1;
  } else if (Charge5 == 1) {
    Charge5 = 0;
    UpdateShiftregister();
    CellVoltage5 = analogRead(CELL5);
    CellVoltage5 = CellVoltage5 * (5.0 / CalCell5Factor);
    Charge5 = 1;
  } else {
    CellVoltage5 = analogRead(CELL5);
    CellVoltage5 = CellVoltage5 * (5.0 / CalCell5Factor);
  }

  if (Load6 == 1) {
    Load6 = 0;
    UpdateShiftregister();
    CellVoltage6 = analogRead(CELL6);
    CellVoltage6 = CellVoltage6 * (5.0 / CalCell6Factor);
    Load6 = 1;
  } else if (Charge6 == 1) {
    Charge6 = 0;
    UpdateShiftregister();
    CellVoltage6 = analogRead(CELL6);
    CellVoltage6 = CellVoltage6 * (5.0 / CalCell6Factor);
    Charge6 = 1;
  } else {
    CellVoltage6 = analogRead(CELL6);
    CellVoltage6 = CellVoltage6 * (5.0 / CalCell6Factor);
  }

  UpdateShiftregister();

}

void Init() {
  pinMode(T_CENTER, INPUT);
  pinMode(FAN, OUTPUT);
  pinMode(T_RIGHT, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(T_UP, INPUT);
  pinMode(T_DOWN, INPUT);
  pinMode(SHCP, OUTPUT);
  pinMode(STCP, OUTPUT);
  pinMode(DS, OUTPUT);
  pinMode(CELL1, INPUT);
  pinMode(CELL2, INPUT);
  pinMode(CELL3, INPUT);
  pinMode(CELL4, INPUT);
  pinMode(CELL5, INPUT);
  pinMode(CELL6, INPUT);

  Shiftregisters.reset();

  Serial.begin(9600);

  Adafruit_SSD1306 display(OLED_RESET);
  display.begin(SSD1306_SWITCHCAPVCC, DisplayAdress);
  display.display();

  if (myAHT10.begin() != true) {
    Serial.println(F("AHT10 sensor defective or not connected!"));
    WarningTone();
  } else {
    Serial.println(F("AHT10 is active."));
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 0);
  display.print(F("ChargeOS"));
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.print(F("V")); display.print(Version);
  display.display();
  delay(500);

  playDoomMelody();

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(12, 0);
  display.print(F("By Marvis"));
  display.setCursor(41, 18);
  display.print(F("Tech"));
  display.display();
  delay(1000);

  BootDoneTone();
}

void PrintVoltageSerial() {
  Serial.print(F("C1: ")); Serial.print(CellVoltage1); Serial.println(F(" V"));
  Serial.print(F("C2: ")); Serial.print(CellVoltage2); Serial.println(F(" V"));
  Serial.print(F("C3: ")); Serial.print(CellVoltage3); Serial.println(F(" V"));
  Serial.print(F("C4: ")); Serial.print(CellVoltage4); Serial.println(F(" V"));
  Serial.print(F("C5: ")); Serial.print(CellVoltage5); Serial.println(F(" V"));
  Serial.print(F("C6: ")); Serial.print(CellVoltage6); Serial.println(F(" V"));
}

void MeasureTemperatre() {
  ResistorTemperature = myAHT10.readTemperature();
}

void PrintTemperatureSerial() {
  Serial.print(F("Temperature: ")); Serial.print(ResistorTemperature); Serial.println(F(" +-0.3C"));
}

void WarningTone() {
  tone(BUZZER, NOTE_D6, 1000);
  delay(1500);
  tone(BUZZER, 0 , 500);
  delay(500);
}

void InformationTone() {
  tone(BUZZER, NOTE_B3, 700);
  delay(500);
  tone(BUZZER, NOTE_CS6 , 900);
  delay(200);
}

void SelectTone() {
  tone(BUZZER, NOTE_CS5, 150);
  delay(150);
}

void BootDoneTone() {
  tone(BUZZER, NOTE_DS6, 200);
  delay(250);
  tone(BUZZER, NOTE_DS6, 200);
  delay(200);
}

void Shiftregister() {
  for (int i = 0; i <= 24; i++) {
    if (ShiftregisterBinOut[i] == 1) {
      Shiftregisters.setPin(i, ON);
    } else {
      Shiftregisters.setPin(i, OFF);
    }
  }
}

void UpdateShiftregister() {
  ShiftregisterBinOut[7] = Load1;
  ShiftregisterBinOut[6] = Load2;
  ShiftregisterBinOut[5] = Load3;
  ShiftregisterBinOut[4] = Load4;
  ShiftregisterBinOut[3] = Load5;
  ShiftregisterBinOut[2] = Load6;

  ShiftregisterBinOut[1] = Charge1;
  ShiftregisterBinOut[0] = Charge2;
  ShiftregisterBinOut[15] = Charge3;
  ShiftregisterBinOut[14] = Charge4;
  ShiftregisterBinOut[13] = Charge5;
  ShiftregisterBinOut[12] = Charge6;

  ShiftregisterBinOut[11] = LED_Green1;
  ShiftregisterBinOut[8] = LED_Green2;
  ShiftregisterBinOut[21] = LED_Green3;

  ShiftregisterBinOut[10] = LED_Red1;
  ShiftregisterBinOut[23] = LED_Red2;
  ShiftregisterBinOut[20] = LED_Red3;

  ShiftregisterBinOut[9] = LED_Blue1;
  ShiftregisterBinOut[22] = LED_Blue2;
  ShiftregisterBinOut[19] = LED_Blue3;

  Shiftregister();
}

void UpdateFanSpeed() {
  if ((ResistorTemperature >= TempMin) && (ResistorTemperature <= TempMax)) {
    FanSpeed = map(ResistorTemperature, TempMin, TempMax, 32, 255);
    analogWrite(FAN, FanSpeed);
    DisplayFanSpeed = map(FanSpeed, 0, 255, 0, 100);
  }
}

void ReadButtons() {
  buttonStateR = digitalRead(T_RIGHT);
  buttonStateU = digitalRead(T_UP);
  buttonStateD = digitalRead(T_DOWN);
  buttonStateM = digitalRead(T_CENTER);
}



///////////////////////////////DOOM melody///////////////////////////////

int const Notes[8][12] = {
  { 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62 },
  { 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123 },
  { 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247 },
  { 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494 },
  { 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988 },
  { 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976 },
  { 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951 },
  { 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902 }
};

#define NOTE_C 0
#define NOTE_CS 1
#define NOTE_D 2
#define NOTE_DS 3
#define NOTE_E 4
#define NOTE_F 5
#define NOTE_FS 6
#define NOTE_G 7
#define NOTE_GS 8
#define NOTE_A 9
#define NOTE_AS 10
#define NOTE_B 11

void playNote(int octave, int note, int duration)
{
  tone(BUZZER, Notes[octave][note]);
  delay(duration);
  noTone(BUZZER);
}

inline void noteDoomBase(int octave, int speed) {
  playNote(octave - 1, NOTE_E, speed / 2);
  delay(speed / 2);
  playNote(octave - 1, NOTE_E, speed);
}

void playDoomMelody() {
  int octave = 3;

  // Fast part
  int speed = 64;

  playNote(octave, NOTE_B, speed);
  playNote(octave, NOTE_G, speed);
  playNote(octave, NOTE_E, speed);
  playNote(octave, NOTE_C, speed);

  playNote(octave, NOTE_E, speed);
  playNote(octave, NOTE_G, speed);
  playNote(octave, NOTE_B, speed);
  playNote(octave, NOTE_G, speed);

  playNote(octave, NOTE_B, speed);
  playNote(octave, NOTE_G, speed);
  playNote(octave, NOTE_E, speed);
  playNote(octave, NOTE_G, speed);

  playNote(octave, NOTE_B, speed);
  playNote(octave, NOTE_G, speed);
  playNote(octave, NOTE_B, speed);
  playNote(octave + 1, NOTE_E, speed);

  // Main theme
  speed = 128;

  noteDoomBase(octave, speed);
  playNote(octave, NOTE_E, speed);

  noteDoomBase(octave, speed);
  playNote(octave, NOTE_D, speed);

  noteDoomBase(octave, speed);
  playNote(octave, NOTE_C, speed);

  noteDoomBase(octave, speed);
  playNote(octave, NOTE_AS, speed);

  noteDoomBase(octave, speed);
  playNote(octave, NOTE_B, speed);
  playNote(octave, NOTE_C, speed);

  noteDoomBase(octave, speed);
  playNote(octave, NOTE_E, speed);

  noteDoomBase(octave, speed);
  playNote(octave, NOTE_D, speed);

  noteDoomBase(octave, speed);
  playNote(octave, NOTE_C, speed);

  noteDoomBase(octave, speed);
  playNote(octave - 1, NOTE_AS, speed * 2);
}
