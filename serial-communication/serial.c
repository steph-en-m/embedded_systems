#include <avr/io.h>
#include <util/delay.h>

void serial_init();
char serial_receive();
void serial_transmit(char x);
 
char temp;
 
#define lcd PORTA
 
void cmd(char x);
void lcd_display(char x);
void lcd_init();
 
int main()
   {
      int i=0;
      DDRA=0XFF;
      DDRB=0XFF;
      serial_init();
      lcd_init();
      while(1)
        {
            i++;
            temp = serial_receive();
	   
            lcd_display(temp);
            if(i==16)
               cmd(0xc0); //move to 2nd row
            else if(i==32)
               {
                   i=0;
                   cmd(0x01);
                }
         }
      return 0;
   }
 
void serial_init()
   {
       //UBRR = 1666;
       UCSRB=(1<<TXEN)|(1<<RXEN);
       UCSRC=(1<<UCSZ1)|(1<<UCSZ0)|(1<<URSEL);
       UBRRL=0x05;
   }

void serial_transmit(char x)
  {
     while(!(UCSRA & (1<<UDRE)));
     UDR= x;
  }
  
char serial_receive()
   {
       while((UCSRA & (1<<RXC))==0);
       temp=UDR;
       return temp;
   }
 
void cmd(char x)
   {
       lcd=x;
       PORTB=(0<<0);
       PORTB=(0<<1);
       PORTB=(1<<2);
       _delay_ms(10);
       PORTB=(0<<2);
 
   }
 
void lcd_display(char x)
  {
     lcd=x;
     PORTB |=(1<<0); //0-RS, 0-RW, 1-EN
     PORTB &=~(1<<1);
     PORTB |=(1<<2); // 1-RS, 0-RW, 0-EN
     _delay_ms(20);
     PORTB &=~(1<<2);
  } 
 
void lcd_init()
  {
     cmd(0x38); //set:8-bit, 2 line, 5x7 Dots 
     cmd(0x0e);
     cmd(0x01); //clear screen
     cmd(0x06); //cursor-direction(L-R)
     cmd(0x80); //force cursor to begin with 1st row
  }
