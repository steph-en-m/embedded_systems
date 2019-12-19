typedef unsigned char uint8_t;
#define MOSI 2
#define MISO 3
#define SCK 1
#define SS 0
#define MSB_FIRST 0
#define LSB_FIRST 1
#define SCL_CLK 100000L
#define BITRATE(TWSR) ((F_CPU / SCL_CLK) - 16) / (2 * pow(4, (TWSR & ((1 << TWPS0) | (1 << TWPS1))))) /* Define bit rate */

void i2c_init() /* I2C initialize function */
{
    TWBR = BITRATE(TWSR = 0x00); /* Get bit rate register value by formula */
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
            ;                              /* Wait until TWI finish its current job (start condition) */
        status = TWSR & 0xF8;              /* Read TWI status register with masking lower three bits */
        if (status != 0x08)                /* Check weather start condition transmitted successfully or not? */
            continue;                      /* If no then continue with start loop again */
        TWDR = write_address;              /* If yes then write SLA+W in TWI data register */
        TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
        while (!(TWCR & (1 << TWINT)))
            ;                 /* Wait until TWI finish its current job (Write operation) */
        status = TWSR & 0xF8; /* Read TWI status register with masking lower three bits */
        if (status != 0x18)   /* Check weather SLA+W transmitted & ack received or not? */
        {
            I2C_Stop(); /* If not then generate stop condition */
            continue;   /* continue with start loop again */
        }
        break; /* If yes then break loop */
    }
}

int I2C_Read_Ack() /* I2C read ack function */
{
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); /* Enable TWI, generation of ack and clear interrupt flag */
    while (!(TWCR & (1 << TWINT)))
        ;        /* Wait until TWI finish its current job (read operation) */
    return TWDR; /* Return received data */
}

int I2C_Read_Nack() /* I2C read nack function */
{
    TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
    while (!(TWCR & (1 << TWINT)))
        ;        /* Wait until TWI finish its current job (read operation) */
    return TWDR; /* Return received data */
}
