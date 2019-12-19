#include <avr/io.h>
#include <util/delay.h>
#define RS 0
#define EN 1

void lcd_comm(char x);

void lcd_clr()
{
    lcd_comm(0x01); /* clear display */
    lcd_comm(0x80); /* cursor at home position */
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

void lcd_string(char *str)
{
    int i;
    for (i = 0; str[i] != 0; i++)
    {
        lcd_data(str[i]);
    }
}
