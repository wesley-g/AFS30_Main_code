#ifndef BG96_H_
#define BG96_H_

#include <Message_handler.h>

/* BG96 network functions */
uint8_t BG96_Startup(void);
void BG96_setting(void);

/* CAT-M */
void Init_CAT_M(void);
void Connect_CAT_M(void);
void Transmit_CAT_M(void);

/*NB-IoT */
void Init_NB_IOT(void);
void Connect_NB_IOT(void);
void Transmit_NB_IOT(void);

/* 2G GPRS */
void Connect_2G_BG96(void);
void Transmit_2G_BG96(void);

/* GPS */
void GPS_BG96(void);
void BG_Network_Check(void);


/* Variables */
uint8_t startup;
char StringURL[150];
char StringPost[150];
int PostLength;

void BG96_setting(void)
{
    uart_puts_BG96("AT+QCFG=\"band\",1,80000,80000,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nwscanseq\",020301,1\r");
    ReceiveCheckQuectel();

}


/* CAT-M Functions */
void Init_CAT_M(void)
{
    Network = 2;    //active network
    MCC = 204;  //Mobile Country Code
    MNC = 8;    //Mobile Network Code
    Size = 128;    //test data size in bytes

    uart_puts_BG96("AT+QCFG=\"nwscanmode\",3,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"roamservice\",1,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"iotopmode\",2,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nbsibscramble\",1\r");
    ReceiveCheckQuectel();

    PowerupCheckBG96(1);  //reset
    Systick_delay(500); //opstart tijd nodig voordat SIM uitgelezen kan worden
}

void Connect_CAT_M(void)
{
    uart_puts_BG96("AT+COPS= 1,2,\"20408\",8\r"); //NL KPN
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QNWINFO\r");
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

void Transmit_CAT_M(void)
{
    uart_puts_BG96("AT+QPING=1,\"8.8.8.8\",15,4\r");
    Systick_delay(2000);
    ReceiveCheckQuectel();
}

/* NB-IoT Functions */
void Init_NB_IOT(void) //T-Mobile
{
    Network = 3;    //active network
    MCC = 204;  //Mobile Country Code
    MNC = 6;    //Mobile Network Code
    Size = 128;    //test data size in bytes

    uart_puts_BG96("AT+QCFG=\"nwscanmode\",0,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"roamservice\",2,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"iotopmode\",0,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nbsibscramble\",0\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+CGDCONT=1,\"IP\",\"nb.inetd.gdsp\"\r");
    Systick_delay(1000);
    ReceiveCheckQuectel();

    PowerupCheckBG96(1);  //reset
    Systick_delay(500); //opstart tijd nodig voordat SIM uitgelezen kan worden
}

void Connect_NB_IOT(void)
{
    uart_puts_BG96("AT+COPS= 1,2,\"20404\",9\r"); //NL KPN
    Systick_delay(2000);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QNWINFO\r");
    Systick_delay(1000);
    ReceiveCheckQuectel();

    while (counter < 5 && SignalStrength == 0)
    {
        uart_puts_BG96("AT+CSQ\r");
        QuectelCSQCheck();
        counter++;
    }

    counter = 0;
}

void Transmit_NB_IOT(void)
{
    uart_puts_BG96("AT+QPING=1,\"8.8.8.8\",15,4\r");
    Systick_delay(3000);
    ReceiveCheckQuectel();
}

/*
void Connect_2G_BG96(void)
{
    uart_puts_BG96("AT+COPS= 1,2,\"20408\",0\r");
    Systick_delay(5000);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QNWINFO\r");
    ReceiveCheckQuectel();
}

void Transmit_2G_BG96(void)
{
    uart_puts_BG96("AT+QIDEACT=1\r");
    Systick_delay(2000);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QHTTPCFG=\"contextid\",1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QICSGP=1,1,\"multimedia.ah.nl\",\"\",\"\",1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QIACT=1\r");
    Systick_delay(2500);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QIACT?\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QHTTPCFG=\"sslctxid\",1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QSSLCFG=\"sslversion\",1,3\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QSSLCFG=\"ciphersuite\",1,0x0035\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QSSLCFG=\"seclevel\",1,0\r");
    ReceiveCheckQuectel();

    sprintf(StringURL, "https://top-connector.binary-glue.com/network/1\r");

    Systick_delay(100);

    uart_puts_BG96("AT+QHTTPURL=47,80\r");
    ConnectCheckQuectel();

    uart_puts_BG96(StringURL);
    ReceiveCheckQuectel();

    PostLength = strlen(StringToSend) - 2;

    sprintf(StringPost, "AT+QHTTPPOST=%d,80\r", PostLength);

    uart_puts_BG96(StringPost); //MESSAGE
    ConnectCheckQuectel();

    uart_puts_BG96(StringToSend);
    Systick_delay(2000);
    ReceiveCheckQuectel();

}*/

/* Startup Function */
uint8_t BG96_Startup(void)
{
    UART_A0_Init();      //BG96 UART Init

    uart_puts_BG96("AT\r");

    if (ReceiveCheckQuectel() != 0)
    {
        PowerupCheckBG96(0); // opstart is klaar
    }

    Systick_delay(500); //opstart tijd nodig voordat SIM uitgelezen kan worden

    uart_puts_BG96("AT+CIMI\r");

    if (ReceiveCheckQuectel() != 0)
    {
        PowerupCheckBG96(1); //1
    }

    uart_puts_BG96("AT+CIMI\r");

    startup = ReceiveCheckQuectel();

    return startup;
}

void GPS_BG96(void)
{
    uart_puts_BG96("AT+QGPS=1\r");
    ReceiveCheckQuectel();

    for(j = 0; j < 10; j++)
    {
        uart_puts_BG96("AT+QGPSLOC=2\r");
        ConnectCheckQuectel(0);
    }

    uart_puts_BG96("AT+QGPSLOC=2\r");
    if(ConnectCheckQuectel(1) == 0)
    {
        for (j = 0; j < 4; j++)
        {
            MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
            Systick_delay(100);
        }

        uart_puts_BG96("AT+QGPSEND\r");
        ReceiveCheckQuectel();
    }

    for (x = 0; x < BUFF_SIZE; x++)
    {
        RXBuffer[x] = 0;
    }
}
/*
void BG_Network_Check(void)
{

    uart_puts_BG96("AT+QIACT?\r");
    if (QuectelOkCheck() == 1)
    {
        Connect_2G_BG96();

        Transmit_2G_BG96();
    }

    uart_puts_BG96("AT+QHTTPURL?\r");
    if (QuectelOkCheck() == 1)
    {
        sprintf(StringURL, "https://top-connector.binary-glue.com/network/1\r");

        Systick_delay(100);

        uart_puts_BG96("AT+QHTTPURL=47,80\r");
        ConnectCheckQuectel();

        uart_puts_BG96(StringURL);
        ReceiveCheckQuectel();
    }

    PostLength = strlen(StringToSend) - 2;

    sprintf(StringPost, "AT+QHTTPPOST=%d,80\r", PostLength);

    uart_puts_BG96(StringPost); //MESSAGE
    ConnectCheckQuectel();

    uart_puts_BG96(StringToSend);
    Systick_delay(3000);
    ReceiveCheckQuectel();

 }*/


#endif /* BG96_H_ */
