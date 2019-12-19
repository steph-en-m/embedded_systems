#include "avr/io.h"
#include "avr/interrupt.h"
#include <util/delay.h>
#include <avr/eeprom.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "lcd.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "terminal.h"
#include "usart.h"

#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#define set_bit(REG, BIT) ((REG) |= (1 << (BIT)))
#define BAUDRATE 9600
#define UBRR_VAL (((F_CPU / (BAUDRATE * 16UL))) - 1)

//=================================================================
int main(void)
{
    char buffer[20];
    char *days[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    i2c_init();

    //unsigned int starttime=millis();
    DDRF = 0xFF;
    DDRB = 0x07;
    // DDRJ = 0x02;
    // DDRD = 0x01;
    lcd_init();

    //Set Min and Max temp
    float max_temp = 29.0;
    float min_temp = 17.0;

    int t = 0, result;
    int i = 0, reloadLCD = 1;
    double runingtime = 1.0000;
    char tmp[3], runduration[10];

    uint8_t high_temp_byte;
    //uint8_t low_temp_byte ;
    char dummy;
    char data;

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
    printstr2vt("Enter number to proceed:\n");
    printstr2vt("1. Check temperature\r\n");
    printstr2vt("2. Check period\r\n");
    printstr2vt("3. Check date\r\n");
    printstr2vt("4. Set min_temp\r\n");
    printstr2vt("5. Set max_temp\r\n");

    while (1)
    {
        RTC_Read_Clock(0); /* Read the clock with second address i.e location is 0 */
        if (hour & TimeFormat12)
        {
            sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
            if (Afternoon(hour))
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
        spi_init(MSB_FIRST);
        set_bit(PORTB, SS);
        spi_write(0x80);
        _delay_ms(160);
        dummy = SPDR;
        spi_write(0x11);      // set at control reg 0x15 to adjust it at one shot conversion .
        CLEAR_BIT(PORTB, SS); // make CE to TC72 as 0 .
        dummy = SPDR;
        _delay_ms(160); // delay to complete temp conversion .
        set_bit(PORTB, SS);
        spi_write(0x02); // send address of temp high byte .
        dummy = SPDR;
        spi_write(0x00); // send dummy output (0x00) to start clock pulses
        high_temp_byte = SPDR;
        spi_write(0X00); //read temp low byte
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
                    printstr2vt("Enter min-temp  :");
                }
                else if (atoi(inputcha) == 5)
                {
                    printstr2vt("Enter max-temp  :");
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
                printstr2vt("Setting maximum temperature...");
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
                printstr2vt("Setting minimum temperature...");
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

        // Handle out of range temperature
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
                    PORTF = 0x10; /*led blink*/
                    _delay_ms(10);
                    PORTF = 0x00;
                    _delay_ms(10);
                    t++;
                }

                lcd_string("TEMP ");
                lcd_string(tmp);
                lcd_string("C");            /* write temperature on 1st line of LCD*/
                lcd_comm(0xC0);             /* Go to 2nd line*/
                lcd_string("OUT OF RANGE"); /* Write string on 2nd line*/
                //lcd_string(tmp);
                for (i = 0; i < 1000; i++)
                {
                    PORTF = 0x04;
                    _delay_ms(5);
                    PORTF = 0x00;
                    _delay_ms(5);
                }
                lcd_clr();
                reloadLCD = 0;
            }
            else /*if(((result<=max_temp) && (result>=min_temp)))*/
            {
                _delay_ms(5);
                lcd_comm(0x80);
                lcd_string(tmp);
                lcd_string(".C");
                lcd_comm(0xC0); /* Go to 2nd line*/
                lcd_string("NORMAL");
            }
        }
        else
        {
            _delay_ms(5);
            lcd_comm(0x80);
            lcd_string(mt);
            lcd_string(".C");
            lcd_comm(0xC0); /* Go to 2nd line*/
            lcd_string("MAXIMUM TEMP");
        }
    }
    return 0;
}
