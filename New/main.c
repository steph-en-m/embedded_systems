#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>

#define MOSI 5
#define MISO 6
#define SS 4
#define SCK 7
#define RS 0
#define EN 1

void spi_init_slave(void);
void spi_init_master(void);
unsigned char spi_tranceiver(unsigned char data);
void putcspi(char cx);
char getcspi(void);
void lcd_comm(char);
void lcd_data(char);
void lcd_init(void);
int gettemp(void);

int main()
{
    spi_init_master();
    spi_init_slave();
    int temp = gettemp();
    // printf("Temp: %c", temp);
    // char temp1 = spi_tranceiver();
    lcd_init();
    lcd_data(temp);
    lcd_data('2');
    lcd_data('8');

    while (1)
    {
    }
    return 0;
}

void putcspi(char cx)
{
    char temp;
    while (!(SPSR & (1 << 5)))
        ;      // wait until write is permissible
    SPDR = cx; // output the byte to SPI
    while (!(SPSR & (1 << 7)))
        ;        // wait until write operation is complete
    temp = SPDR; // clear the SPIF flag
}

char getcspi(void)
{
    //while (!(SPSR & (1 << 5)))
    //; // wait until write is permissible
    //SPDR = 0x00; // trigger 8 SCK  pulses to shift in data
    while (!(SPSR & (1 << 7)))
        ;        // wait until a byte has been shifted in
    return SPDR; // return the character
}

int gettemp(void)
{
    // while (!(SPSR & (1 << 5)))
    // ;        // wait until write is permissible
    // SPDR = 0x00; // trigger 8 SCK  pulses to shift in data
    while (!(SPSR & (1 << 7)))
        ;        // wait until a byte has been shifted in
    return SPDR; // return the character
}

void spi_init_slave(void)
{
    DDRB = (1 << MISO); // Set MISO
    SPCR = (1 << SPE);  // Enable
}

void spi_init_master(void)
{
    DDRB = (1 << MOSI) | (1 << SCK);

    // Enable SPI, Set as Master
    // Prescaler: Fosc/16, Enable Interrupts
    //The MOSI, SCK pins are as per ATMega8

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

    // Enable Global Interrupts
    // sei();
}

unsigned char spi_tranceiver(unsigned char data)
{
    // Load data into the buffer
    SPDR = data;

    //Wait until transmission complete
    while (!(SPSR & (1 << SPIF)))
        ;

    // Return received data
    return (SPDR);
}

void lcd_comm(char x)
{
    PORTA = x;
    PORTC &= ~(1 << RS);
    PORTC |= 1 << EN;
    _delay_ms(5);
    PORTC &= ~(1 << EN);
}

void lcd_data(char x)
{
    PORTA = x;
    PORTC |= 1 << RS;
    PORTC |= 1 << EN;
    _delay_ms(5);
    PORTC &= ~(1 << EN);
}

void lcd_init(void)
{
    DDRA = 0xFF;
    DDRC = 0x03;
    lcd_comm(0x38);
    lcd_comm(0x06);
    lcd_comm(0x0E);
    lcd_comm(0x01);
    lcd_comm(0x80);
}
