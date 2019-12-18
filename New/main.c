#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define MOSI 5
#define MISO 6
#define SS 4
#define SCK 7
#define RS 0
#define EN 1

void spienable(void);
void putcspi(char cx);
char getcspi(void);
void lcd_comm(char);
void lcd_data(char);
void lcd_init(void);

int main()
{
    spienable();
    char temp = getcspi();
    lcd_init();
    lcd_data(temp);
    lcd_data('C');

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
    while (!(SPSR & (1 << 5)))
        ;        // wait until write is permissible
    SPDR = 0x00; // trigger 8 SCK  pulses to shift in data
    while (!(SPSR & (1 << 7)))
        ;        // wait until a byte has been shifted in
    return SPDR; // return the character
}

void spienable(void)
{
    DDRB = (1 << MISO); // Set M0SI
    SPCR = (1 << SPE);  // Enable
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