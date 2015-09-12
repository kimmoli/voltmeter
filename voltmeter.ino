/* 
Chines wireless current/voltage meter receiver
2015 Kimmo Lindholm

Thanks to Nick Gammon, for SPI slave code http://www.gammon.com.au/spi 
*/


#include <SPI.h>

unsigned char buf [100];
volatile byte pos;
volatile boolean process_it;
int count;

void setup (void)
{
  Serial.begin (115200);
  SPCR |= bit (SPE);

  pinMode(MISO, OUTPUT);

  pos = 0;
  process_it = false;
  count = 10;

  SPI.attachInterrupt();
  
  Serial.println("ready");

}



ISR (SPI_STC_vect)
{
byte c = SPDR; 
  
  if (pos < sizeof buf)
  {
    buf [pos++] = c;
    if (pos > 10)
      process_it = true;
  }
}

void loop (void)
{
  delay(2);
  if (process_it)
  {
/*
    // Debug prints
    int i;
    for (i=0 ; i<pos ; i++)
    {
      Serial.print (buf[i], HEX);
      Serial.print (" ");
    }
    Serial.println();
*/
/*
    Frame from current/voltage measurement unit is like:
    40 40 40 A E6 4 1A 0 D 0 3 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 40 0 
    
    Check that the preamble matches, then extract voltage and current from
    every 10th frame
    
    voltage and current is 16-bit value, LSB = 10mV (10mA)
*/
    if (buf[0] == 0x40 && buf[1] == 0x40 && buf[2] == 0x40)
    {
      if (++count >= 10)
      {
        float voltage = (float)(buf[4] + (buf[5]<<8))/100.0;
        float current = (float)(buf[6] + (buf[7]<<8))/100.0;
        Serial.print(voltage, 2);
        Serial.print(" V ");
        Serial.print(current, 2);
        Serial.println(" A ");
        count = 0;
      }
    }
    pos = 0;
    process_it = false;
  }  
}  
