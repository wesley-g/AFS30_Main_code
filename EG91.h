#ifndef EG91_H_
#define EG91_H_

/* BG96 network functions */
uint8_t EG91_Startup(void);

/* 4G LTE */
void Init_4G(void);
void Connect_4G(void);
void Transmit_4G(void);

/* 3G UMTS */
void Init_3G(void);
void Connect_3G(void);
void Transmit_3G(void);

/* 2G GPRS */
void Init_2G_EG91(void);
void Connect_2G_EG91(void);
void Transmit_2G_EG91(void);

/* 4G Functions */
void Init_4G(void)
{
    uart_puts_EG91("AT+CFUN=0\r");
    Systick_delay(3000);    // delay nodig
    ReceiveCheckQuectel();
}

void Connect_4G(void)
{
    uart_puts_EG91("AT+CFUN=1\r");  //werkt~
    Systick_delay(2000);    // delay nodig
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+COPS=?\r");
    Systick_delay(25000);
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+COPS= 1,2,\"20408\",7\r"); //NL KPN
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QNWINFO\r");
    ReceiveCheckQuectel();
}

void Transmit_4G(void)
{
    uart_puts_EG91("AT+QPING=1,\"8.8.8.8\",15,4\r");
    ReceiveCheckQuectel();
}


/* 3G Functions */
void Init_3G(void)
{
    uart_puts_EG91("AT+CFUN=0\r");
    Systick_delay(2000);    // delay nodig
    ReceiveCheckQuectel();
}

void Connect_3G(void)
{
    uart_puts_EG91("AT+CFUN=1\r");  //werkt~
    Systick_delay(2000);    // delay nodig
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+COPS=?\r");
    Systick_delay(12500);
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+COPS= 1,2,\"20408\",2\r"); //NL KPN
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QNWINFO\r");
    ReceiveCheckQuectel();
}

void Transmit_3G(void)
{

}

/* 2G Functions */
void Init_2G_EG91(void)
{
    uart_puts_EG91("AT+CFUN=0\r");
    Systick_delay(2000);    // delay nodig
    ReceiveCheckQuectel();
}

void Connect_2G_EG91(void)
{
    uart_puts_EG91("AT+CFUN=1\r");  //werkt~
    Systick_delay(2000);    // delay nodig
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+COPS=?\r");
    Systick_delay(12500);
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+COPS= 1,2,\"20408\",0\r"); //NL KPN
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QNWINFO\r");
    ReceiveCheckQuectel();
}

void Transmit_2G_EG91(void)
{

}

/* Startup Function */
uint8_t EG91_Startup(void)
{
    UART_A3_Init(); // EG91 UART Init

    uart_puts_EG91("AT\r");

    if (ReceiveCheckQuectel() != 0)
    {
        PowerupCheckEG91(0); // Powerup
    }

    uart_puts_EG91("AT+CIMI\r");

    if (ReceiveCheckQuectel() != 0)
    {
        PowerupCheckEG91(1); // Reset

    }

    uart_puts_EG91("AT+CIMI\r");

    startup = ReceiveCheckQuectel();

    return startup;
}

#endif /* EG91_H_ */
