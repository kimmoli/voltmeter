
#include <t3spi.h>

//Initialize T3SPI class as SPI_SLAVE
T3SPI SPI_SLAVE;

//The number of integers per data packet
//MUST be the same as defined on the MASTER device
#define dataLength 37 
// 37

//Initialize the arrays for incoming data
//volatile uint8_t data[dataLength] = {};
volatile uint8_t data[dataLength] = {};
uint8_t buf[100] = {};
int pc;
int count = 0;
int led = 13;

void setup(){
  
  Serial.begin(115200);
  pinMode(led, OUTPUT);  
  //Begin SPI in SLAVE (SCK pin, MOSI pin, MISO pin, CS pin)
  SPI_SLAVE.begin_SLAVE(ALT_SCK, ALT_MOSI, ALT_MISO, ALT_CS0);
  
  //Set the CTAR0_SLAVE0 (Frame Size, SPI Mode)
  //SPI_SLAVE.setCTAR_SLAVE(8, SPI_MODE0);
  SPI_SLAVE.setCTAR_SLAVE(8, SPI_MODE0);
  pc = 0;
  //Enable the SPI0 Interrupt
  NVIC_ENABLE_IRQ(IRQ_SPI0);
    
}

void loop()
{
  delay(1000);
}

int ok = 0;
uint8_t b0;
uint8_t b1;
uint8_t b2;

//Interrupt Service Routine to handle incoming data
void spi0_isr(void){
  
  //Function to handle data
  //SPI_SLAVE.rx8 (data, dataLength);
  SPI_SLAVE.rx8(data, 1);
  SPI_SLAVE.packetCT = 0;
  b2 = b1;
  b1 = b0;
  b0 = data[0];
  
  buf[pc++] = data[0];
  if (pc > 90) pc = 0;
  if (b0 == 0x40 && b1 == 0x40 && b2 == 0x40)
  {
    pc = 0;
    ok = 1;
    return;
  }
  
  if (ok && pc >= 5)
  {
    ok = 0;
    pc = 0;
    digitalWrite(led, HIGH);
    if (++count >= 10)
    {
      float voltage = (float)(buf[1] + (buf[2]<<8))/100.0;
      float current = (float)(buf[3] + (buf[4]<<8))/100.0;
      /* Only print to serial port if it is used in host */
      if (Serial.dtr())
      {
        Serial.print(voltage, 2);
        Serial.print(" V ");
        Serial.print(current, 2);
        Serial.println(" A");
      }
      count = 0;
    }
    digitalWrite(led, LOW);
  }
}

