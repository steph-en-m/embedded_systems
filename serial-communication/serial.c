#include<avr/io.h>
#include<util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#define fosc 8000000 //oscillator freq(clockspeed)
#define baud 9600
#define myubrr 5 //fosc/16/baud-1

#define lcd PORTA
#define SCK PINB1
#define MISO PINB3
#define MOSI PINB2
#define SS PINB0

void serial_init(int ubrr);
char serial_receive();
void serial_transmit(char data);
int ReadTemp(void);
char info;
void cmd(char x);
void lcd_display(char x);
void lcd_init();
void SPIinit(void);

 
int main()
   {
      DDRA=0XFF;
      DDRC=0XFF;
      //int i=0;
      int temp;
      serial_init(myubrr);
      SPIinit();
      lcd_init();
      while(1)
        {
	   serial_transmit("Set Temperature: ");
	   temp = ReadTemp();
	   serial_transmit(temp);
	   break;
	   
	   
	   /* lcd char read from VT
	   i++;
	   temp = serial_receive();
           lcd_display(temp);
           if(i==16)
              cmd(0xc0);
           else if(i==32)
              {
                  i=0;
                  cmd(0x01);
               }*/
         }
      return 0;
   }
 
void serial_init(int ubrr)
   {
      UCSR0A = 0x00; //clear status reg a
      UBRR0H =(char)(ubrr>>8);  //initializing ubrrh
      UBRR0L = (char) ubrr;
      UCSR0B |= (1<<TXEN0)|(1<<RXEN0); //enable communication
      UCSR0C |= (1<<USBS0)|(3<<UCSZ00); //2-stopbits,8-databits C-reg.config
 
   }

void serial_transmit(char data)
  {
     /*wait for transmit buffer to empty*/
     while(!(UCSR0A & (1<<UDRE0)));
	
     /*load data into the buffer*/
     UDR0 = data;
  }
  
char serial_receive()
   {
      /*wait for data receipt*/
      while((UCSR0A & (1<<RXC))==0);
      
      /*get received data from the buffer*/
      info=UDR0;
      return info;
   }

/*initializing SPI*/
void SPIinit(void)
   {
      DDRB |= (1<<MOSI)|(1<<SCK)|(1<<SS);	// Set MOSI , SCK , and SS output
      DDRB &= ~(1<<MISO);
      SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0); // Enable SPI, Master, set clock rate fck/128
      PORTB |= (1<<SS) | (1<<MISO);
   }

/*SPI read*/
int SPIReadByte(int addr)
      {
	 SPDR = addr;	//Load byte to Data register
	 _delay_ms(30);
	 //while(!(SPSR & (1<<SPIF))); // Wait for transmission complete. The 7th bit goes to one when data is done getting exchanged
	 addr=SPDR;
	 return addr;
      }
      
/*SPI write*/
void SPIWriteByte( int byteword)
      {
	 SPDR = byteword; // put the byteword into data register
	 _delay_ms(30);
	 while(!(SPSR & (1<<SPIF)));	// Wait for transmission complete
	 byteword=SPDR;	// clear SPIF
      }

/*Temperature reading*/
int ReadTemp(void) 
      {
	 int temp;
	 PORTB &= ~(1<<SS);
	 SPIWriteByte(0xFA); // Call on register address for MSB temperature byte
	 temp = SPIReadByte(0xFF); // Exchange a garbage byte for the temperature byte
	 PORTB |= (1<<SS);
	 return temp; // Return the 8 bit temperature
}      
void cmd(char x)
   {
       lcd=x;
       PORTC=(0<<0);
       PORTC=(0<<1);
       PORTC=(1<<2);
       _delay_ms(10);
       PORTC=(0<<2);
 
   }
 
void lcd_display(char x)
  {
     lcd=x;
     PORTC |=(1<<0); //0-RS, 0-RW, 1-EN
     PORTC &=~(1<<1);
     PORTC |=(1<<2); // 1-RS, 0-RW, 0-EN
     _delay_ms(20);
     PORTC &=~(1<<2);
  } 
 
void lcd_init()
  {
     cmd(0x38); //set:8-bit, 2 line, 5x7 Dots 
     cmd(0x0e);
     cmd(0x01); //clear screen
     cmd(0x06); //cursor-direction(L-R)
     cmd(0x80); //force cursor to begin with 1st row
  }
