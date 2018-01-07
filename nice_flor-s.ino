/*
Данный скетч принимает код с брелока NICE FLOR-S
*/

#define rxPin 2 //пин приемника

unsigned long p_len, p_len_prev;
volatile byte level=255;
volatile unsigned long last, len;
byte p_level;
   
 byte state = 0;
 byte i = 0; // count of bits stored
 byte R[13];  
 byte nicereceive[52];
 
void setup() {
  
 Serial.begin(9600); //скорость у UNO 9600
 Serial.println("NICE LOGGER"); 
 pinMode(rxPin, INPUT);
 attachInterrupt(0, rx_int, CHANGE); //первый аргумент для UNO - 0
 interrupts();

}

void loop()
{ 
  if(level!=255)
  {
    noInterrupts();
    p_level=level;
    p_len=len;
    len=0;
    level=255;
    interrupts();    
    flors();
    p_len_prev=p_len;
  }
  
  if (state == 100) 
  {

    R[0] = 0;
      for(int rNumber=0; rNumber < 12; rNumber++) {
         for(int m=0 + rNumber*4; m < 4 + rNumber*4; m++){
          if (nicereceive[m] == 0) {  R[rNumber] = (R[rNumber] << 1) | 0; } else   {
            R[rNumber] = (R[rNumber] << 1) | 1; }
            R[rNumber] = 0;
            }
      }
      // 6 8 9 A B C D E F
      // 5 8 9 A B C D E F
      R[0] = 0;
    if (((R[0] == 0x1) && ((R[1] == 0xE) || (R[1] == 0xF) || (R[1] == 0xC) || (R[1] == 0xD) || (R[1] == 0xA) || (R[1] == 0xB) || (R[1] == 0x8) ||  (R[1] == 0x9) || (R[1] == 0x6))) ||  
    ((R[0] == 0x2) && ((R[1] == 0xD) || (R[1] == 0xC) || (R[1] == 0xF) || (R[1] == 0xE) || (R[1] == 0x9) || (R[1] == 0x8) || (R[1] == 0xB) ||  (R[1] == 0xA) || (R[1] == 0x5))))   { 
      Serial.print("NICE FLOR-S: ");
      for (int i=0; i < 12; i++)
        Serial.print(R[i], HEX); 
      Serial.println(""); 
}
 
 state=0;
  }
}

void rx_int()
{
  if(level!=255) return;
  
  len=micros()-last;
  last=micros();
  
  if(digitalRead(rxPin)==HIGH) level=0;
    else level=1; 
} 
 
void flors() {

switch(state) 
 {
 case 0:
 
      if(p_level) break;
      state=1;
      i=0;
      for(int m=0;m<52;m++){ nicereceive[m] = 0; }
      break;
       
  case 1:
       
      if (((p_len > 1400) && (p_len < 1600)) && (p_level))  {
           state = 2;         
           break; 
      }
        else
      {
       state = 0;
       break;     
      }
      break;
     
 case 2:

  if(!p_level) break; 
  
    if ((p_len > 900) && (p_len < 1100) )  // valid 1
    {
      nicereceive[i] = (nicereceive[i] << 1) | 1; 
      i++;
    }
    else if  ((p_len > 400) && (p_len < 600) )  // valid 0
    {
      nicereceive[i] = (nicereceive[i] << 1) | 0;
      i++;
    }
    if  (i == 52) state = 100;
    break;
    
   }  
}
