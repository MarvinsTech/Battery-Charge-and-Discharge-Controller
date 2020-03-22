#ifndef HC595_h
#define HC595_h

#define OFF 0
#define ON 1
#define TOGGLE 3
#define MSBF 0
#define LSBF 1

#include "arduino.h"

class HC595 {
  public:
    HC595(int count, int latchPin, int clockPin, int dataPin); 
	void isLeastSignificantBitFirst();                         
	void isMostSignificantBitFirst();                          
	void reset();                                              
    void setPin(int pin, int action);                          
    void setPins(int from, int to, int action);                
    void send();                                               
    int getState(int pin);                                     
    String toString();                                         
	int lastPin();                                              
  private:
    void sendToChips();                     
	void reverseSendToChips();              
	String byteToString(int data);          
    int MYarray[8] = {0,0,0,0,0,0,0,0};     
    int MYmask[8]= {1,2,4,8,16,32,64,128}; 
    int MYchipCount;                        
    int MYlatchPin;                         
    int MYclockPin;                        
    int MYdataPin;                          
	int MYway = MSBF;                       
	int MYlastPin;                          
};

#endif
