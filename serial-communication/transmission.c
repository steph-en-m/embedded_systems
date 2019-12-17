//serial transmission
//send data from microcontroller to the virtual terminal

void delay(int x);
void serial_init();
void serial_transmit(unsigned char x);
unsigned char temp;
 
int main()
    {
       serial_init();
       while(1)
          {
	    temp = serial_receive();
            serial_transmit(temp);
	    temp = 0; 
            delay(1000);
            //serial_transmit(0x0d);
            //delay(400);
 
          }
        return 0;
   }
 
void serial_init()
  {
     UCSRA=0x00;
     DDRA=0xff;
     UCSRB|=(1<<TXEN)|(1<<RXEN);
     UCSRC|=(1<<UCSZ1)|(1<<UCSZ0)|(1<<URSEL);
     UBRRL=51;
     UBRRH=51<<8;
  }
 
 
void serial_transmit(unsigned char x)
  {
     while(!(UCSRA & (1<<UDRE)));
     UDR= x;
  }
  
void serial_receive()
  {
     while((UCSRA & (1<<RXC))==0);
     return UDR;
   }
  
void delay(int x)
   {
        x=x*2;
        _delay_ms(x);
   }
 
