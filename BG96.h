#ifndef BG96_H_
#define BG96_H_

#define BUFF_SIZE 200

/* BG96 network functions */
uint8_t BG96_Startup(void);
void BG96_setting(void);

/* CAT-M */
void Init_CAT_M(void);
void Connect_CAT_M(void);
void Transmit_CAT_M(uint8_t UDPsize);

/*NB-IoT */
void Init_NB_IOT(void);
void Connect_NB_IOT(void);
void Transmit_NB_IOT(uint8_t UDPsize);

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
char MQTLogin[150];
char MQTSUB[150];
char MQTPUB[150];
char MQTSEND[150];
char constant[20];
char TransmitSize[500];
int PostLength;

char ClientID[] = "16e70430-f480-11e8-9c33-75e6b356cec4";
char Username[] = "3b83e490-f3c0-11e8-a254-e163efaadfe8";
char Password[] = "5eaef0ad685ca558a7a9317d60c758976874d3b1";

void BG96_setting(void)
{
    uart_puts_BG96("AT+QCFG=\"band\",1,80000,80,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nwscanseq\",030201,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nwscanmode\",3,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"roamservice\",1,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nbsibscramble\",0\r");
    ReceiveCheckQuectel();
}

/* CAT-M Functions */
void Init_CAT_M(void)
{
    Network = 2;    //active network
    MCC = 204;  //Mobile Country Code
    MNC = 8;    //Mobile Network Code

    uart_puts_BG96("AT+CFUN=0\r"); //NL KPN
    Systick_delay(2000);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"iotopmode\",0,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+CGDCONT=1,\"IP\",\"item.webtrial.m2m\"\r"); //KPN
    Systick_delay(1000);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+CFUN=1\r"); //NL KPN
    Systick_delay(1000);
    ReceiveCheckQuectel();

    /*
    PowerupCheckBG96(1);  //reset
    Systick_delay(500); //opstart tijd nodig voordat SIM uitgelezen kan worden
    */
}

void Connect_CAT_M(void)
{
    uart_puts_BG96("AT+COPS= 1,2,\"20408\",8\r"); //NL KPN
    Systick_delay(1000);
    ReceiveCheckQuectel();

    while (counter < 5 && SignalStrength == 0)
    {
        uart_puts_EG91("AT+CSQ\r");
        QuectelCSQCheck();
        counter++;
    }

    uart_puts_BG96("AT+QNWINFO\r");
    Systick_delay(100);
    ReceiveCheckQuectel();

    counter = 0;
}

void Transmit_CAT_M(uint8_t UDPsize)
{
    uart_puts_BG96("AT+QIACT=1\r");
    Systick_delay(100);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QIOPEN=1,0,\"TCP\",\"220.180.239.212\",8009,0,1\r"); //NL KPN
    Systick_delay(100);
    ReceiveCheckQuectel();

    for (s = 0; s < UDPsize; s++)
    {
        sprintf(TransmitSize, "1%s", TransmitSize);
    }

    uart_puts_BG96("AT+QISEND=0\r");
    Systick_delay(100);
    ReceiveCheckQuectel();

    uart_puts_BG96(TransmitSize); //Stuur UDP bericht
    Systick_delay(100);
    ReceiveCheckQuectel();

    MAP_UART_transmitData(EUSCI_A0_BASE, 0x1a);// CTRL+Z
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QICLOSE\r");
    Systick_delay(100);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QIDEACT=1\r");
    Systick_delay(100);
    ReceiveCheckQuectel();
}

void Init_NB_IOT(void) //T-Mobile
{
    Network = 3;    //active network
    MCC = 204;  //Mobile Country Code
    MNC = 4;    //Mobile Network Code

    uart_puts_BG96("AT+QCFG=\"iotopmode\",1,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"roamservice\",1,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+CGDCONT=1,\"IP\",\"cdp.iot.t-mobile.nl\"\r"); //T-Mobile
    Systick_delay(1000);
    ReceiveCheckQuectel();

    //PowerupCheckBG96(1);  //reset
    //Systick_delay(500); //opstart tijd nodig voordat SIM uitgelezen kan worden
}

void Connect_NB_IOT(void)
{
    uart_puts_BG96("AT+COPS= 1,2,\"20416\",9\r"); //Vodafone
    Systick_delay(4000);
    ReceiveCheckQuectel();

    while (counter < 5 && SignalStrength == 0)
    {
        uart_puts_BG96("AT+CSQ\r");
        QuectelCSQCheck();
        counter++;
    }

    uart_puts_BG96("AT+QNWINFO\r");
    Systick_delay(1000);
    ReceiveCheckQuectel();

    counter = 0;
}


/* NB-IoT Functions */
/*
void Init_NB_IOT(void) //VODAFONE
{
    Network = 3;    //active network
    MCC = 204;  //Mobile Country Code
    MNC = 6;    //Mobile Network Code
    Size = 128;    //test data size in bytes

    uart_puts_BG96("AT+QCFG=\"iotopmode\",2,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+CGDCONT=1,\"IP\",\"nb.inetd.gdsp\"\r"); //VODAFONE
    Systick_delay(1000);
    ReceiveCheckQuectel();

    //PowerupCheckBG96(1);  //reset
    //Systick_delay(500); //opstart tijd nodig voordat SIM uitgelezen kan worden
}

void Connect_NB_IOT(void)
{
    uart_puts_BG96("AT+COPS= 1,2,\"20404\",9\r"); //Vodafone
    Systick_delay(4000);
    ReceiveCheckQuectel();

    while (counter < 5 && SignalStrength == 0)
    {
        uart_puts_BG96("AT+CSQ\r");
        QuectelCSQCheck();
        counter++;
    }

    uart_puts_BG96("AT+QNWINFO\r");
    Systick_delay(1000);
    ReceiveCheckQuectel();

    counter = 0;
}*/

void Transmit_NB_IOT(uint8_t UDPsize)
{
    uart_puts_BG96("AT+QIOPEN=1,0,\"UDP\",\"172.27.131.100\",15683\r");// Open UDP
    Systick_delay(100);
    ReceiveCheckQuectel();

    for (s = 0; s < UDPsize; s++)
    {
        sprintf(TransmitSize, "%s1", TransmitSize);
    }

    uart_puts_BG96("AT+QISEND=0,1\r");
    Systick_delay(100);
    ReceiveCheckQuectel();

    uart_puts_BG96(TransmitSize);//Stuur UDP bericht
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QICLOSE=0\r");//Sluit UDP
    Systick_delay(100);
    ReceiveCheckQuectel();

    Systick_delay(3000);
}


void Connect_2G_BG96(void)
{
    uart_puts_BG96("AT+COPS= 1,2,\"20408\",0\r");
    Systick_delay(5000);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QNWINFO\r");
    ReceiveCheckQuectel();
}

/*
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
}
*/

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

// MyCayenne ID Username password

void connect_MyCayenne(void)
{
    uart_puts_BG96("AT+QMTOPEN=0,\"mqtt.mydevices.com\",1883\r");
    Systick_delay(2000);
    ReceiveCheckQuectel();

    sprintf(MQTLogin,"AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r",ClientID,Username,Password);

    uart_puts_BG96(MQTLogin);
    Systick_delay(2000);
    ReceiveCheckQuectel();
}

void send_MyCayenne(int channel, int data, int unit) //subscribe en publish
{
    /* Cayenne MyDevices setup
     *
     * channel 0 = PMIC temp
     * channel 1 = XO temp
     * channel 2 = PA temp
     * channel 3 = Vbat
     * channel 4 = rssi
     * channel 5 = rsrp
     * channel 6 = sinr
     * channel 7 = rsrq
     *
     * unit 1 = temp (Celcius)
     * unit 2 = voltage (v)
     * unit 3 = rssi (dbm)
     * unit 4 = snr (db)
     * unit 5 = batt (%)
     */

    if(unit == 1)
    {
         sprintf(constant,"temp,c");
    }
    else if (unit == 2)
    {
        sprintf(constant,"voltage,mv");
    }
    else if (unit == 3)
    {
        sprintf(constant,"rssi,dbm");
    }
    else if (unit == 4)
    {
        sprintf(constant,"snr,db");
    }
    else if (unit == 5)
    {
        sprintf(constant,"batt,p");
    }

    sprintf(MQTSUB,"AT+QMTSUB=0,1,\"v1/%s/things/%s/data/%d\",1\r",Username,ClientID,channel);

    uart_puts_BG96(MQTSUB);
    Systick_delay(2000);
    ReceiveCheckQuectel();

    sprintf(MQTPUB,"AT+QMTPUB=0,1,1,0,\"v1/%s/things/%s/data/%d\"\r",Username,ClientID,channel);

    uart_puts_BG96(MQTPUB);
    Systick_delay(2000);
    ReceiveCheckQuectel();

    sprintf(MQTSEND,"%s=%d",constant,data);

    uart_puts_BG96(MQTSEND);
    MAP_UART_transmitData(EUSCI_A0_BASE, 0x1A); //ctrl+z

    Systick_delay(2000);
    ReceiveCheckQuectel();
}

void close_MyCayenne(void)
{
    uart_puts_BG96("AT+QMTDISC=0\r");
    Systick_delay(2000);
    ReceiveCheckQuectel();
}


#endif /* BG96_H_ */
