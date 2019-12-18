#include <avr/eeprom.h>

void setMaxAndMinTemp(uint8_t Min, uint8_t Max)
{
    eeprom_write_byte(23, Min)
        eeprom_write_byte(24, Max)
}