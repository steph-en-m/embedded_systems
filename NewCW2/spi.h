#define CLEAR_BIT(REG, BIT) ((REG) &= ~(1 << (BIT)));
#define SET_BIT(REG, BIT) ((REG) |= (1 << (BIT)))

void spi_init(char data_dr)
{
    DDRB |= (1 << MOSI) | (1 << SCK) | (1 << SS);
    CLEAR_BIT(DDRB, MISO);
    SPCR |= (1 << SPE) | (1 << MSTR);
    //SPCR |= (1<<SPR0) ;    /* SPI clock speed = Fosc/ 16 . */
    (data_dr == 1) ? SET_BIT(SPCR, DORD) : CLEAR_BIT(SPCR, DORD);
    // SPCR |= 1<<DORD ; // LSB first .
    SPSR |= 1 << SPI2X;
}

char spi_read(void) /* SPI read data function */
{
    //SPDR = 0xFF;
    while (!(SPSR & (1 << SPIF)))
        ; // Spin until reception is complete
    return (SPDR);
}

void spi_write(char data)
{
    SPDR = data;
    while (!(SPSR & (1 << SPIF)))
        ; // spin util transmission is complete
}