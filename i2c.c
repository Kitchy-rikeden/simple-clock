#include<xc.h>
#define _XTAL_FREQ 125000

void I2C_Master_Init(const unsigned long c){
  SSP1CON1 = 0x28;                //SSP1 Module as Master
  SSP1CON2 = 0x00;
  SSP1CON3 = 0x00;
  SSP1ADD = (_XTAL_FREQ/(4*c))-1; //Setting Clock Speed
  SSP1STAT = 0x80;
  
  SSP1CLKPPS = 0x0E;              //PPS Settings
  SSP1DATPPS = 0x0C;              // SCL : RB6
  RB6PPS = 0x18;                  // SDA : RB4
  RB4PPS = 0x19;
}
void I2C_Master_Wait(){
  while ((SSP1STAT & 0x04) || (SSP1CON2 & 0x1F)); //Transmit is in progress
    
}
void I2C_Master_Start(){
  I2C_Master_Wait();    
  SSP1CON2bits.SEN = 1;           //Initiate start condition
}
void I2C_Master_RepeatedStart(){
  I2C_Master_Wait();
  SSP1CON2bits.RSEN = 1;          //Initiate repeated start condition
}
void I2C_Master_Stop(){
  I2C_Master_Wait();
  SSP1CON2bits.PEN = 1;           //Initiate stop condition
}
void I2C_Master_Write(unsigned d){
  I2C_Master_Wait();
  SSP1BUF = d;                    //Write data to SSP1BUF
}
unsigned short I2C_Master_Read(unsigned short a){
  unsigned short temp;
  I2C_Master_Wait();
  SSP1CON2bits.RCEN = 1;
  I2C_Master_Wait();
  temp = SSP1BUF;                 //Read data from SSP1BUF
  I2C_Master_Wait();
  SSP1CON2bits.ACKDT = a;         //Acknowledge bit		1:Not Ack	0:Ack
  SSP1CON2bits.ACKEN = 1;         //Acknowledge sequence
  return temp;
}
void SendI2C(char adrs, char data){
    I2C_Master_Start();
    I2C_Master_Write(adrs<<1);      //SlaveAdress + Write
    I2C_Master_Write(data);         //mainly RegisterAdress
    I2C_Master_Stop();
}
void CmdI2C(char adrs, char reg, char data){
    I2C_Master_Start();
    I2C_Master_Write(adrs<<1);      //SlaveAdress + Write
    I2C_Master_Write(reg);          //RegisterAdress
    I2C_Master_Write(data);
    I2C_Master_Stop();
}
void GetDataI2C(char adrs, char* buf, char cnt){
    I2C_Master_Start();
    I2C_Master_Write((adrs<<1)+1);      //SlaveAdress + Read
    for(char i=0; i<cnt-1; i++)
        buf[i] = I2C_Master_Read(0);    //ACK
    buf[cnt-1] = I2C_Master_Read(1);    //NACK
    I2C_Master_Stop();
}