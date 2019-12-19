
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#if __GNUC__
#include "avr/io.h"
#include "avr/interrupt.h"
#else
#include "ioavr.h"
#endif
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <time.h>
//#include <date>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "I2C_Master_H_file.h"
//EEPROM
#include <avr/eeprom.h>
//i2c=============================================
#define Device_Write_address 0xD0 /* Define RTC DS1307 slave address for write operation */
#define Device_Read_address 0xD1  /* Make LSB bit high of slave address for read operation */
#define TimeFormat12 0x40		  /* Define 12 hour format */
#define AMPM 0x20

int second, minute, hour, day, date, month, year;
//========================rtc=====================================================
#include <math.h>																					  /* Include math function */
#define SCL_CLK 100000L																				  /* Define SCL clock frequency */
#define BITRATE(TWSR) ((F_CPU / SCL_CLK) - 16) / (2 * pow(4, (TWSR & ((1 << TWPS0) | (1 << TWPS1))))) /* Define bit rate */

void I2C_Init();							   /* I2C initialize function */
uint8_t I2C_Start(char write_address);		   /* I2C start function */
uint8_t I2C_Repeated_Start(char read_address); /* I2C repeated start function */
void I2C_Stop();							   /* I2C stop function */
void I2C_Start_Wait(char write_address);	   /* I2C start wait function */
uint8_t I2C_Write(char data);				   /* I2C write function */
int I2C_Read_Ack();							   /* I2C read ack function */
int I2C_Read_Nack();						   /* I2C read nack function */
//========================end rtc====================================================
//================================================================================
#define LCD_Data_Dir DDRF	  /* Define LCD data port direction */
#define LCD_Command_Dir DDRG   /* Define LCD command port direction register */
#define LCD_Data_Port PORTF	/* Define LCD data port */
#define LCD_Command_Port PORTG /* Define LCD data port */
#define RS PG0				   /* Define Register Select (data/command reg.)pin */
#define RW PG1				   /* Define Read/Write signal pin */
#define EN PG2				   /* Define Enable signal pin */
#define BAUDRATE 9600
#define UBRR_VAL (((F_CPU / (BAUDRATE * 16UL))) - 1)

/*  =================== lcd functions ======================== */
void LCD_Command(unsigned char cmnd)
{
	LCD_Data_Port = cmnd;
	LCD_Command_Port &= ~(1 << RS); /* RS=0 command reg. */
	LCD_Command_Port &= ~(1 << RW); /* RW=0 Write operation */
	LCD_Command_Port |= (1 << EN);  /* Enable pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(3);
}

void LCD_Char(char char_data) /* LCD data write function */
{
	LCD_Data_Port = char_data;
	LCD_Command_Port |= (1 << RS);  /* RS=1 Data reg. */
	LCD_Command_Port &= ~(1 << RW); /* RW=0 write operation */
	LCD_Command_Port |= (1 << EN);  /* Enable Pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(1);
}

void LCD_Init(void) /* LCD Initialize function */
{
	LCD_Command_Dir = 0xFF; /* Make LCD command port direction as o/p */
	LCD_Data_Dir = 0xFF;	/* Make LCD data port direction as o/p */
	_delay_ms(10);			/* LCD Power ON delay always >15ms */

	LCD_Command(0x38); /* Initialization of 16X2 LCD in 8bit mode */
	LCD_Command(0x0C); /* Display ON Cursor OFF */
	LCD_Command(0x06); /* Auto Increment cursor */
	LCD_Command(0x01); /* Clear display */
	LCD_Command(0x80); /* Cursor at home position */
}

void LCD_String(char *str) /* Send string to LCD function */
{
	int i;
	for (i = 0; str[i] != 0; i++) /* Send each char of string till the NULL */
	{
		LCD_Char(str[i]);
	}
}

void LCD_String_xy(char row, char pos, char *str) /* Send string to LCD with xy position */
{
	if (row == 0 && pos < 16)
		LCD_Command((pos & 0x0F) | (0x80)); /* Command of first row and required position<16 */
	else if (row == 1 && pos < 16)
		LCD_Command((pos & 0x0F) | (0xC0)); /* Command of first row and required position<16 */
	LCD_String(str);						/* Call LCD string function */
}

void LCD_Clear()
{
	LCD_Command(0x01); /* clear display */
	LCD_Command(0x80); /* cursor at home position */
}

/*  =================== sensor functions ======================== */
typedef unsigned char uint8_t;
#define MOSI 2
#define MISO 3
#define SCK 1
#define SS 0
#define MSB_FIRST 0
#define LSB_FIRST 1
/*MACROS*/
#define SET_BIT(REG, BIT) ((REG) |= (1 << (BIT)))
#define CLEAR_BIT(REG, BIT) ((REG) &= ~(1 << (BIT)))
#define delay_ms(X) _delay_ms(X)
#define BUTTON 0

void SPI_Init(char data_dr) /* SPI Initialize function */
{
	DDRB |= (1 << MOSI) | (1 << SCK) | (1 << SS);
	CLEAR_BIT(DDRB, MISO);
	SPCR |= (1 << SPE) | (1 << MSTR); /* Enable SPI module , controller is master. */
	//SPCR |= (1<<SPR0) ;    /* SPI clock speed = Fosc/ 16 . */
	(data_dr == 1) ? SET_BIT(SPCR, DORD) : CLEAR_BIT(SPCR, DORD);
	// SPCR |= 1<<DORD ; // LSB first .
	SPSR |= 1 << SPI2X;
}

char SPI_Read(void) /* SPI read data function */
{
	//SPDR = 0xFF;
	while (!(SPSR & (1 << SPIF)))
		;		   /* Wait till reception complete */
	return (SPDR); /* Return received data */
}

void SPI_Write(char data) /* SPI write data function */
{
	SPDR = data; /* Write data to SPI data register */
	while (!(SPSR & (1 << SPIF)))
		; /* Wait till transmission complete */
}

/*  ===========terminal functions========*/

//#define UBRR_VAL	30
/* USART Buffer Defines */
#define USART_RX_BUFFER_SIZE 8 /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART_TX_BUFFER_SIZE 8 /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART_RX_BUFFER_MASK (USART_RX_BUFFER_SIZE - 1)
#define USART_TX_BUFFER_MASK (USART_TX_BUFFER_SIZE - 1)

#if (USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK)
#error RX buffer size is not a power of 2
#endif
#if (USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK)
#error TX buffer size is not a power of 2
#endif

/* Static Variables */
static unsigned char USART_RxBuf[USART_RX_BUFFER_SIZE];
static volatile unsigned char USART_RxHead;
static volatile unsigned char USART_RxTail;
static unsigned char USART_TxBuf[USART_TX_BUFFER_SIZE];
static volatile unsigned char USART_TxHead;
static volatile unsigned char USART_TxTail;

/* Prototypes */
void USART1_Init(unsigned int ubrr_val);
unsigned char USART1_Receive(void);
void USART1_Transmit(unsigned char data);

bool enteredIsNum(unsigned char cha)
{
	int i = 0;
	char numbers[] = "0123456789.";
	while (numbers[i] != '\0')
	{
		if (cha == numbers[i])
		{
			return true;
		}
		i++;
	}
	return false;
}
void printstr2vt(char *stringg)
{
	int y = 0;
	while (stringg[y] != '\0')
	{
		USART1_Transmit(stringg[y]);
		y++;
	}
}

//=====================RTC FUNCTIONS===============================

bool IsItPM(char hour_)
{
	if (hour_ & (AMPM))
		return 1;
	else
		return 0;
}

void RTC_Read_Clock(char read_clock_address)
{
	I2C_Start(Device_Write_address);		 /* Start I2C communication with RTC */
	I2C_Write(read_clock_address);			 /* Write address to read */
	I2C_Repeated_Start(Device_Read_address); /* Repeated start with device read address */

	second = I2C_Read_Ack(); /* Read second */
	minute = I2C_Read_Ack(); /* Read minute */
	hour = I2C_Read_Nack();  /* Read hour with Nack */
	I2C_Stop();				 /* Stop i2C communication */
}

void RTC_Read_Calendar(char read_calendar_address)
{
	I2C_Start(Device_Write_address);
	I2C_Write(read_calendar_address);
	I2C_Repeated_Start(Device_Read_address);

	day = I2C_Read_Ack();   /* Read day */
	date = I2C_Read_Ack();  /* Read date */
	month = I2C_Read_Ack(); /* Read month */
	year = I2C_Read_Nack(); /* Read the year with Nack */
	I2C_Stop();				/* Stop i2C communication */
}

//=================================================================
int main(void)
{
	//===========================RTC_INT===================================
	char buffer[20];
	char *days[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

	I2C_Init();
	//=====================================================================
	//===========================lcd int===================================
	//unsigned int starttime=millis();
	DDRH = 0xFF;
	DDRB = 0x07;
	DDRJ = 0x02;
	DDRD = 0x01;
	LCD_Init();
	//=====================================================================

	//float temp=2.0;
	float max_temp = 29.0;
	float min_temp = 17.0;
	int t = 0, result;
	int i = 0, reloadLCD = 1;
	double runingtime = 1.0000;
	char tmp[3], runduration[10];
	/*--------------------------*/
	uint8_t high_temp_byte;
	//uint8_t low_temp_byte ;
	char dummy;
	char data;
	/*--------------------------*/
	int waiting4number = 0;
	int waiting4min = 0;
	int waiting4max = 0;
	char inputnum[2];
	char inputcha[2];
	char inputnums[6];
	char inputchas[10];
	float min_tem, max_tem;
	USART1_Init(UBRR_VAL);

	sei();

	/* Set the baud rate to 9600 bps using 8MHz internal RC oscillator */
	USART1_Init(UBRR_VAL);
	printstr2vt("\n  check_temperature : 1 \r");
	printstr2vt("\n  check_period_____ : 2 \r");
	printstr2vt("\n  check_date_______ : 3 \r");
	printstr2vt("\n  Set min_temp_____ : 4 \r");
	printstr2vt("\n  Set max_temp_____ : 5 \r");
	printstr2vt("_________________________________________\r");

	for (;;)
	{
		//========================RTC=======================================
		RTC_Read_Clock(0); /* Read the clock with second address i.e location is 0 */
		if (hour & TimeFormat12)
		{
			sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
			if (IsItPM(hour))
				strcat(buffer, "PM");
			else
				strcat(buffer, "AM");
			//lcd_print_xy(0,0,buffer);
		}

		else
		{
			sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
			//lcd_print_xy(0,0,buffer);
		}
		RTC_Read_Calendar(3); /* Read the calender with day address i.e location is 3 */
		sprintf(buffer, "%02x/%02x/%02x %3s ", date, month, year, days[day - 1]);

		//==================================================================
		runingtime = runingtime + 1.5;
		SPI_Init(MSB_FIRST);
		SET_BIT(PORTB, SS);
		SPI_Write(0x80);
		_delay_ms(160);
		dummy = SPDR;
		SPI_Write(0x11);	  // set at control reg 0x15 to adjust it at one shot conversion .
		CLEAR_BIT(PORTB, SS); // make CE to TC72 as 0 .
		dummy = SPDR;
		_delay_ms(160); // delay to complete temp conversion .
		SET_BIT(PORTB, SS);
		SPI_Write(0x02); // send address of temp high byte .
		dummy = SPDR;
		SPI_Write(0x00); // send dummy output (0x00) to start clock pulses
		high_temp_byte = SPDR;
		SPI_Write(0X00); //read temp low byte
		CLEAR_BIT(PORTB, SS);
		result = high_temp_byte + 16;
		itoa(result, tmp, 10);

		/*------------USART--------------*/
		if ((data = USART1_Receive()) != '\0')
		{
			//printstr2vt(tmp);
			if ((data == ':'))
			{
				//read which Character is in inputcha
				if (atoi(inputcha) == 4)
				{
					printstr2vt("  Enter min-temp  :");
				}
				else if (atoi(inputcha) == 5)
				{
					printstr2vt("  enter max-temp  :");
				}
				else if (atoi(inputcha) == 1)
				{
					printstr2vt(" Real_time_temp: ");
					printstr2vt(tmp);
					printstr2vt(" C.");
				}
				else if (atoi(inputcha) == 3)
				{
					printstr2vt(buffer);
				}
				else if (atoi(inputcha) == 2)
				{
					itoa(runingtime, runduration, 10);
					printstr2vt("  Runtime is ");
					printstr2vt(runduration);
					printstr2vt(" s.");
				}
			}
			else if (((data == '1') || (data == '2') || (data == '3')) && (!waiting4number))
			{
				inputcha[0] = data;
			}
			else if ((data == '4') && (!waiting4number))
			{
				waiting4number = 1;
				waiting4min = 1;
				waiting4max = 0;
				inputcha[0] = data;
				strcat(inputchas, inputcha);
			}
			else if ((data == '5') && (!waiting4number))
			{
				waiting4number = 1;
				waiting4max = 1;
				waiting4min = 0;
				inputcha[0] = data;
				strcat(inputchas, inputcha);
			}
			else if ((data == ';') && (waiting4number && waiting4max))
			{ /*set max*/
				max_temp = atoi(inputnums);
				eeprom_write_byte((uint8_t *)23, max_temp);
				waiting4number = 0;
				reloadLCD = 1;
				/*clear the inputnums variable*/
				sprintf(inputchas, "");
				sprintf(inputnums, "");
				printstr2vt(" setting max....Done.");
			}
			else if ((data == ';') && (waiting4number && waiting4min))
			{ /*set min*/
				min_temp = atoi(inputnums);
				eeprom_write_byte((uint8_t *)20, min_temp);
				waiting4number = 0;
				reloadLCD = 1;
				/*clear the inputnums variable*/
				sprintf(inputchas, "");
				sprintf(inputnums, "");
				printstr2vt(" setting min....Done.");
			}
			else
			{
				if (waiting4number && enteredIsNum(data))
				{
					inputnum[0] = data;
					strcat(inputnums, inputnum);
				}
			}
			//USART1_Transmit('[');
			//USART1_Transmit(data);
			//USART1_Transmit(']');
		}
		//-----------end of usart ---------

		//==========================setting for temperature out range========
		max_tem = eeprom_read_byte((uint8_t *)23);
		min_tem = eeprom_read_byte((uint8_t *)20);
		char mt[4];
		itoa(max_tem, mt, 10);
		if (reloadLCD && (result != 156))
		{
			if (((result > max_tem) | (result < min_tem)))
			{

				while (t < 4)
				{
					PORTH = 0x10; /*led blink*/
					_delay_ms(10);
					PORTH = 0x00;
					_delay_ms(10);
					t++;
				}

				LCD_String("TEMP ");
				LCD_String(tmp);
				LCD_String("C");			/* write temperature on 1st line of LCD*/
				LCD_Command(0xC0);			/* Go to 2nd line*/
				LCD_String("OUT OF RANGE"); /* Write string on 2nd line*/
				//LCD_String(tmp);
				for (i = 0; i < 1000; i++)
				{
					PORTH = 0x04;
					_delay_ms(5);
					PORTH = 0x00;
					_delay_ms(5);
				}
				LCD_Clear();
				reloadLCD = 0;
			}
			else /*if(((result<=max_temp) && (result>=min_temp)))*/
			{
				_delay_ms(5);
				LCD_Command(0x80);
				LCD_String(tmp);
				LCD_String(".C");
				LCD_Command(0xC0); /* Go to 2nd line*/
				LCD_String("NORMAL");
			}
		}
		else
		{
			_delay_ms(5);
			LCD_Command(0x80);
			LCD_String(mt);
			LCD_String(".C");
			LCD_Command(0xC0); /* Go to 2nd line*/
			LCD_String("MAXIMUM TEMP");
		}
		//===================================================================
	}
	return 0;
}

/* Initialize UART */
/* Initialize USART */
void USART1_Init(unsigned int ubrr_val)
{
	unsigned char x;

	/* Set the baud rate */
	UBRR1H = (unsigned char)(ubrr_val >> 8);
	UBRR1L = (unsigned char)ubrr_val;

	/* Enable USART receiver and transmitter */
	UCSR1B = ((1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1));

	/* For devices in which UBRRH/UCSRC shares the same location
	* eg; ATmega16, URSEL should be written to 1 when writing UCSRC
	* 
	*/
	/* Set frame format: 8 data 2stop */
	UCSR1C = (1 << USBS1) | (1 << UCSZ11) | (1 << UCSZ10);

	/* Flush receive buffer */
	x = 0;

	USART_RxTail = x;
	USART_RxHead = x;
	USART_TxTail = x;
	USART_TxHead = x;
}
ISR(USART1_RX_vect)

{
	unsigned char data;
	unsigned char tmphead;

	/* Read the received data */
	data = UDR1;
	/* Calculate buffer index */
	tmphead = (USART_RxHead + 1) & USART_RX_BUFFER_MASK;
	/* Store new index */
	USART_RxHead = tmphead;

	if (tmphead == USART_RxTail)
	{
		/* ERROR! Receive buffer overflow */
	}
	/* Store received data in buffer */
	USART_RxBuf[tmphead] = data;
}

ISR(USART1_UDRE_vect)

{
	unsigned char tmptail;

	/* Check if all data is transmitted */
	if (USART_TxHead != USART_TxTail)
	{
		/* Calculate buffer index */
		tmptail = (USART_TxTail + 1) & USART_TX_BUFFER_MASK;
		/* Store new index */
		USART_TxTail = tmptail;
		/* Start transmission */
		UDR1 = USART_TxBuf[tmptail];
	}
	else
	{
		/* Disable UDRE interrupt */
		UCSR1B &= ~(1 << UDRIE1);
	}
}

unsigned char USART1_Receive(void)
{
	unsigned char tmptail;

	/* Wait for incoming data */
	while (USART_RxHead == USART_RxTail)
		;
	/* Calculate buffer index */
	tmptail = (USART_RxTail + 1) & USART_RX_BUFFER_MASK;
	/* Store new index */
	USART_RxTail = tmptail;
	/* Return data */
	return USART_RxBuf[tmptail];
}
void USART1_Transmit(unsigned char data)
{
	unsigned char tmphead;

	/* Calculate buffer index */
	tmphead = (USART_TxHead + 1) & USART_TX_BUFFER_MASK;
	/* Wait for free space in buffer */
	while (tmphead == USART_TxTail)
		;
	/* Store data in buffer */
	USART_TxBuf[tmphead] = data;
	/* Store new index */
	USART_TxHead = tmphead;
	/* Enable UDRE interrupt */
	UCSR1B |= (1 << UDRIE1);
}

//===================================i2c Master====================================
void I2C_Init() /* I2C initialize function */
{
	TWBR = BITRATE(TWSR = 0x00); /* Get bit rate register value by formula */
}

uint8_t I2C_Start(char write_address) /* I2C start function */
{
	uint8_t status;									  /* Declare variable */
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); /* Enable TWI, generate start condition and clear interrupt flag */
	while (!(TWCR & (1 << TWINT)))
		;							   /* Wait until TWI finish its current job (start condition) */
	status = TWSR & 0xF8;			   /* Read TWI status register with masking lower three bits */
	if (status != 0x08)				   /* Check weather start condition transmitted successfully or not? */
		return 0;					   /* If not then return 0 to indicate start condition fail */
	TWDR = write_address;			   /* If yes then write SLA+W in TWI data register */
	TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1 << TWINT)))
		;				  /* Wait until TWI finish its current job (Write operation) */
	status = TWSR & 0xF8; /* Read TWI status register with masking lower three bits */
	if (status == 0x18)   /* Check weather SLA+W transmitted & ack received or not? */
		return 1;		  /* If yes then return 1 to indicate ack received i.e. ready to accept data byte */
	if (status == 0x20)   /* Check weather SLA+W transmitted & nack received or not? */
		return 2;		  /* If yes then return 2 to indicate nack received i.e. device is busy */
	else
		return 3; /* Else return 3 to indicate SLA+W failed */
}

uint8_t I2C_Repeated_Start(char read_address) /* I2C repeated start function */
{
	uint8_t status;									  /* Declare variable */
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); /* Enable TWI, generate start condition and clear interrupt flag */
	while (!(TWCR & (1 << TWINT)))
		;							   /* Wait until TWI finish its current job (start condition) */
	status = TWSR & 0xF8;			   /* Read TWI status register with masking lower three bits */
	if (status != 0x10)				   /* Check weather repeated start condition transmitted successfully or not? */
		return 0;					   /* If no then return 0 to indicate repeated start condition fail */
	TWDR = read_address;			   /* If yes then write SLA+R in TWI data register */
	TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1 << TWINT)))
		;				  /* Wait until TWI finish its current job (Write operation) */
	status = TWSR & 0xF8; /* Read TWI status register with masking lower three bits */
	if (status == 0x40)   /* Check weather SLA+R transmitted & ack received or not? */
		return 1;		  /* If yes then return 1 to indicate ack received */
	if (status == 0x20)   /* Check weather SLA+R transmitted & nack received or not? */
		return 2;		  /* If yes then return 2 to indicate nack received i.e. device is busy */
	else
		return 3; /* Else return 3 to indicate SLA+W failed */
}

void I2C_Stop() /* I2C stop function */
{
	TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN); /* Enable TWI, generate stop condition and clear interrupt flag */
	while (TWCR & (1 << TWSTO))
		; /* Wait until stop condition execution */
}

void I2C_Start_Wait(char write_address) /* I2C start wait function */
{
	uint8_t status; /* Declare variable */
	while (1)
	{
		TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); /* Enable TWI, generate start condition and clear interrupt flag */
		while (!(TWCR & (1 << TWINT)))
			;							   /* Wait until TWI finish its current job (start condition) */
		status = TWSR & 0xF8;			   /* Read TWI status register with masking lower three bits */
		if (status != 0x08)				   /* Check weather start condition transmitted successfully or not? */
			continue;					   /* If no then continue with start loop again */
		TWDR = write_address;			   /* If yes then write SLA+W in TWI data register */
		TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
		while (!(TWCR & (1 << TWINT)))
			;				  /* Wait until TWI finish its current job (Write operation) */
		status = TWSR & 0xF8; /* Read TWI status register with masking lower three bits */
		if (status != 0x18)   /* Check weather SLA+W transmitted & ack received or not? */
		{
			I2C_Stop(); /* If not then generate stop condition */
			continue;   /* continue with start loop again */
		}
		break; /* If yes then break loop */
	}
}

uint8_t I2C_Write(char data) /* I2C write function */
{
	uint8_t status;					   /* Declare variable */
	TWDR = data;					   /* Copy data in TWI data register */
	TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1 << TWINT)))
		;				  /* Wait until TWI finish its current job (Write operation) */
	status = TWSR & 0xF8; /* Read TWI status register with masking lower three bits */
	if (status == 0x28)   /* Check weather data transmitted & ack received or not? */
		return 0;		  /* If yes then return 0 to indicate ack received */
	if (status == 0x30)   /* Check weather data transmitted & nack received or not? */
		return 1;		  /* If yes then return 1 to indicate nack received */
	else
		return 2; /* Else return 2 to indicate data transmission failed */
}

int I2C_Read_Ack() /* I2C read ack function */
{
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); /* Enable TWI, generation of ack and clear interrupt flag */
	while (!(TWCR & (1 << TWINT)))
		;		 /* Wait until TWI finish its current job (read operation) */
	return TWDR; /* Return received data */
}

int I2C_Read_Nack() /* I2C read nack function */
{
	TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1 << TWINT)))
		;		 /* Wait until TWI finish its current job (read operation) */
	return TWDR; /* Return received data */
}

//=================================================================================
