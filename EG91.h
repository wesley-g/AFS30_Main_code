#ifndef EG91_H_
#define EG91_H_


/* BG96 network functions */
uint8_t EG91_Startup(void);

/* 4G LTE */
void Init_4G(void);
void Connect_4G(void);
void Transmit_4G(void);

/* 3G UMTS */
//void Init_3G(void);
//void Connect_3G(void);
//void Transmit_3G(void);

/* 2G GPRS */
//void Init_2G_EG91(void);
//void Connect_2G_EG91(void);
//void Transmit_2G_EG91(void);

void EG_Network_Check(void);
void Init_Transmit_Cloud(void);

int xx;


void Init_4G(void)
{
    Network = 4;    //active network
    MCC = 204;  //Mobile Country Code
    MNC = 8;    //Mobile Network Code
    Size = 128;    //test data size in bytes
    //Sequence = 0;

    uart_puts_EG91("AT+CFUN=0\r");
    Systick_delay(3000);    // delay nodig
    ReceiveCheckQuectel();
}

void Connect_4G(void)
{
    uart_puts_EG91("AT+CFUN=1\r");  //werkt~
    Systick_delay(2000);    // delay nodig
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+COPS= 1,2,\"20408\",0\r"); //NL KPN
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QNWINFO\r");
    Systick_delay(1000);
    ReceiveCheckQuectel();

    while (counter < 5 && SignalStrength == 0)
    {
        uart_puts_EG91("AT+CSQ\r");
        QuectelCSQCheck();
        counter++;
    }

    counter = 0;

}

void Transmit_4G(void)
{
    uart_puts_EG91("AT+QPING=1,\"8.8.8.8\",15,4\r");
    Systick_delay(2000);
    ReceiveCheckQuectel();
}

void Init_Transmit_Cloud(void)
{
    uart_puts_EG91("AT+QIDEACT=1\r");
    Systick_delay(2000);
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QHTTPCFG=\"contextid\",1\r");
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QICSGP=1,1,\"multimedia.ah.nl\",\"\",\"\",1\r");
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QIACT=1\r");
    Systick_delay(2500);
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QIACT?\r");
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QHTTPCFG=\"sslctxid\",1\r");
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QSSLCFG=\"sslversion\",1,3\r");
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QSSLCFG=\"ciphersuite\",1,0x0035\r");
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QSSLCFG=\"seclevel\",1,0\r");
    ReceiveCheckQuectel();
}

/* 3G Functions */
/*
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

    uart_puts_EG91("AT+COPS= 1,2,\"20408\",2\r"); //NL KPN
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QNWINFO?\r");
    ReceiveCheckQuectel();
}

void Transmit_3G(void)
{

}*/

/* 2G Functions */
/*
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

    uart_puts_EG91("AT+COPS= 1,2,\"20408\",0\r"); //NL KPN
    ReceiveCheckQuectel();

    uart_puts_EG91("AT+QNWINFO\r");
    ReceiveCheckQuectel();
}

void Transmit_2G_EG91(void)
{

}
*/
/* Startup Function */
uint8_t EG91_Startup(void)
{
    UART_A3_Init(); // EG91 UART Init

    uart_puts_EG91("AT\r");

    if (ReceiveCheckQuectel() != 0)
    {
        PowerupCheckEG91(0); // Powerup
    }

    Systick_delay(1000);

    uart_puts_EG91("AT+CIMI\r");

    if (ReceiveCheckQuectel() != 0)
    {
        PowerupCheckEG91(1); // 1  Reset
    }

    uart_puts_EG91("AT+CIMI\r");

    startup = ReceiveCheckQuectel();

    return startup;
}

void EG_Network_Check(void)
{
    uart_puts_EG91("AT+QIACT?\r");
    if (QuectelOkCheck() == 1)
    {
        Init_Transmit_Cloud();
    }

    uart_puts_EG91("AT+QHTTPURL?\r");
    if (QuectelOkCheck() == 1)
    {
        sprintf(StringURL, "https://top-connector.binary-glue.com/network/1\r");

        Systick_delay(100);

        uart_puts_EG91("AT+QHTTPURL=47,80\r");
        ConnectCheckQuectel(0);

        uart_puts_EG91(StringURL);
        ReceiveCheckQuectel();
    }

    Message_counter();

    Sequence = m_characters;

    for (xx = 0; xx < message2; xx++)
    {

        Create_Message();

        PostLength = strlen(StringToSend) - 2;

        sprintf(StringPost, "AT+QHTTPPOST=%d,80\r", PostLength);

        uart_puts_EG91 (StringPost); //MESSAGE
        ConnectCheckQuectel(0);

        uart_puts_EG91 (StringToSend);
        Systick_delay(5000);
        ReceiveCheckQuectel();

        //Sequence++;
    }
}




#endif /* EG91_H_ */
