#define AMPM 0x20
#define Device_Write_address 0xD0 /* Define RTC DS1307 slave address for write operation */

#define Device_Read_address 0xD1 /* Make LSB bit high of slave address for read operation */ /* Define SCL clock frequency */

#define TimeFormat12 0x40 /* Define 12 hour format */

int second, minute, hour, day, date, month, year;

void I2C_Init();                               /* I2C initialize function */
uint8_t I2C_Start(char write_address);         /* I2C start function */
uint8_t I2C_Repeated_Start(char read_address); /* I2C repeated start function */
void I2C_Stop();                               /* I2C stop function */
void I2C_Start_Wait(char write_address);       /* I2C start wait function */
uint8_t I2C_Write(char data);                  /* I2C write function */
int I2C_Read_Ack();                            /* I2C read ack function */
int I2C_Read_Nack();                           /* I2C read nack function */
//=====================RTC FUNCTIONS===============================

bool Afternoon(char hour_)
{
    if (hour_ & (AMPM))
        return 1;
    else
        return 0;
}

void RTC_Read_Clock(char read_clock_address)
{
    I2C_Start(Device_Write_address);         /* Start I2C communication with RTC */
    I2C_Write(read_clock_address);           /* Write address to read */
    I2C_Repeated_Start(Device_Read_address); /* Repeated start with device read address */

    second = I2C_Read_Ack(); /* Read second */
    minute = I2C_Read_Ack(); /* Read minute */
    hour = I2C_Read_Nack();  /* Read hour with Nack */
    I2C_Stop();              /* Stop i2C communication */
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
    I2C_Stop();             /* Stop i2C communication */
}

uint8_t I2C_Start(char write_address) /* I2C start function */
{
    uint8_t status;                                   /* Declare variable */
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); /* Enable TWI, generate start condition and clear interrupt flag */
    while (!(TWCR & (1 << TWINT)))
        ;                              /* Wait until TWI finish its current job (start condition) */
    status = TWSR & 0xF8;              /* Read TWI status register with masking lower three bits */
    if (status != 0x08)                /* Check weather start condition transmitted successfully or not? */
        return 0;                      /* If not then return 0 to indicate start condition fail */
    TWDR = write_address;              /* If yes then write SLA+W in TWI data register */
    TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
    while (!(TWCR & (1 << TWINT)))
        ;                 /* Wait until TWI finish its current job (Write operation) */
    status = TWSR & 0xF8; /* Read TWI status register with masking lower three bits */
    if (status == 0x18)   /* Check weather SLA+W transmitted & ack received or not? */
        return 1;         /* If yes then return 1 to indicate ack received i.e. ready to accept data byte */
    if (status == 0x20)   /* Check weather SLA+W transmitted & nack received or not? */
        return 2;         /* If yes then return 2 to indicate nack received i.e. device is busy */
    else
        return 3; /* Else return 3 to indicate SLA+W failed */
}

uint8_t I2C_Write(char data) /* I2C write function */
{
    uint8_t status;                    /* Declare variable */
    TWDR = data;                       /* Copy data in TWI data register */
    TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
    while (!(TWCR & (1 << TWINT)))
        ;                 /* Wait until TWI finish its current job (Write operation) */
    status = TWSR & 0xF8; /* Read TWI status register with masking lower three bits */
    if (status == 0x28)   /* Check weather data transmitted & ack received or not? */
        return 0;         /* If yes then return 0 to indicate ack received */
    if (status == 0x30)   /* Check weather data transmitted & nack received or not? */
        return 1;         /* If yes then return 1 to indicate nack received */
    else
        return 2; /* Else return 2 to indicate data transmission failed */
}

uint8_t I2C_Repeated_Start(char read_address) /* I2C repeated start function */
{
    uint8_t status;                                   /* Declare variable */
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); /* Enable TWI, generate start condition and clear interrupt flag */
    while (!(TWCR & (1 << TWINT)))
        ;                              /* Wait until TWI finish its current job (start condition) */
    status = TWSR & 0xF8;              /* Read TWI status register with masking lower three bits */
    if (status != 0x10)                /* Check weather repeated start condition transmitted successfully or not? */
        return 0;                      /* If no then return 0 to indicate repeated start condition fail */
    TWDR = read_address;               /* If yes then write SLA+R in TWI data register */
    TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
    while (!(TWCR & (1 << TWINT)))
        ;                 /* Wait until TWI finish its current job (Write operation) */
    status = TWSR & 0xF8; /* Read TWI status register with masking lower three bits */
    if (status == 0x40)   /* Check weather SLA+R transmitted & ack received or not? */
        return 1;         /* If yes then return 1 to indicate ack received */
    if (status == 0x20)   /* Check weather SLA+R transmitted & nack received or not? */
        return 2;         /* If yes then return 2 to indicate nack received i.e. device is busy */
    else
        return 3; /* Else return 3 to indicate SLA+W failed */
}