#define CLEAR_BIT(REG, BIT) ((REG) &= ~(1 << (BIT)));
#define SET_BIT(REG, BIT) ((REG) |= (1 << (BIT)))

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
        ;          /* Wait till reception complete */
    return (SPDR); /* Return received data */
}

void SPI_Write(char data) /* SPI write data function */
{
    SPDR = data; /* Write data to SPI data register */
    while (!(SPSR & (1 << SPIF)))
        ; /* Wait till transmission complete */
}