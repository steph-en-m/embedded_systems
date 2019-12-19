
void USART1_Init(unsigned int ubrr_val)
{
    unsigned char x;

    /* Set the baud rate */
    UBRR1H = (unsigned char)(ubrr_val >> 8);
    UBRR1L = (unsigned char)ubrr_val;

    /* Enable USART receiver and transmitter */
    UCSR1B = ((1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1));

    /* For devices in which UBRRH/UCSRC shares the same location
	* eg; ATmega16, URSEL should be written to 1 when writing UCSRC
	* 
	*/
    /* Set frame format: 8 data 2stop */
    UCSR1C = (1 << USBS1) | (1 << UCSZ11) | (1 << UCSZ10);

    /* Flush receive buffer */
    x = 0;

    USART_RxTail = x;
    USART_RxHead = x;
    USART_TxTail = x;
    USART_TxHead = x;
}

ISR(USART1_RX_vect)
{
    unsigned char data;
    unsigned char tmphead;

    /* Read the received data */
    data = UDR1;
    /* Calculate buffer index */
    tmphead = (USART_RxHead + 1) & USART_RX_BUFFER_MASK;
    /* Store new index */
    USART_RxHead = tmphead;

    if (tmphead == USART_RxTail)
    {
        /* ERROR! Receive buffer overflow */
    }
    /* Store received data in buffer */
    USART_RxBuf[tmphead] = data;
}

ISR(USART1_UDRE_vect)
{
    unsigned char tmptail;

    /* Check if all data is transmitted */
    if (USART_TxHead != USART_TxTail)
    {
        /* Calculate buffer index */
        tmptail = (USART_TxTail + 1) & USART_TX_BUFFER_MASK;
        /* Store new index */
        USART_TxTail = tmptail;
        /* Start transmission */
        UDR1 = USART_TxBuf[tmptail];
    }
    else
    {
        /* Disable UDRE interrupt */
        UCSR1B &= ~(1 << UDRIE1);
    }
}

unsigned char USART1_Receive(void)
{
    unsigned char tmptail;

    /* Wait for incoming data */
    while (USART_RxHead == USART_RxTail)
        ;
    /* Calculate buffer index */
    tmptail = (USART_RxTail + 1) & USART_RX_BUFFER_MASK;
    /* Store new index */
    USART_RxTail = tmptail;
    /* Return data */
    return USART_RxBuf[tmptail];
}
void USART1_Transmit(unsigned char data)
{
    unsigned char tmphead;

    /* Calculate buffer index */
    tmphead = (USART_TxHead + 1) & USART_TX_BUFFER_MASK;
    /* Wait for free space in buffer */
    while (tmphead == USART_TxTail)
        ;
    /* Store data in buffer */
    USART_TxBuf[tmphead] = data;
    /* Store new index */
    USART_TxHead = tmphead;
    /* Enable UDRE interrupt */
    UCSR1B |= (1 << UDRIE1);
}