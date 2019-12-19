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
