#ifndef BG96_H_
#define BG96_H_

/* BG96 network functions */
uint8_t BG96_Startup(void);

/* CAT-M */
void Init_CAT_M(void);
void Connect_CAT_M(void);
void Transmit_CAT_M(void);

/*NB-IoT */
void Init_NB_IOT(void);
void Connect_NB_IOT(void);
void Transmit_NB_IOT(void);

/* 2G GPRS */
void Init_2G_BG96(void);
void Connect_2G_BG96(void);
void Transmit_2G_BG96(void);


/* Variables */
uint8_t startup;


/* CAT-M Functions */
void Init_CAT_M(void)
{
    uart_puts_BG96("AT+CFUN=0\r");
    Systick_delay(2000);    // delay nodig
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"band\",1,80000,80000,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"iotopmode\",0,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nwscanseq\",020202,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nwscanmode\",3,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nbsibscramble\",1\r");
    ReceiveCheckQuectel();

    //PowerupCheckBG96(1);  //reset
    //Systick_delay(500); //opstart tijd nodig voordat SIM uitgelezen kan worden

    //check setting op goeie waarde toevoegen?
    uart_puts_BG96("AT+QCFG=\"nwscanseq\"\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nwscanmode\"\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"band\"\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"roamservice\"\r");
    ReceiveCheckQuectel();
}

void Connect_CAT_M(void)
{
    uart_puts_BG96("AT+CFUN=1\r");  //werkt~
    Systick_delay(2000);    // delay nodig
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+COPS=?\r");
    Systick_delay(12500);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+COPS= 1,2,\"20408\",8\r"); //NL KPN
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QNWINFO\r");
    ReceiveCheckQuectel();
}

void Transmit_CAT_M(void)
{
    uart_puts_BG96("AT+QPING=1,\"8.8.8.8\",15,4\r");
    ReceiveCheckQuectel();
}

/* NB-IoT Functions */
void Init_NB_IOT(void) //T-Mobile
{
    uart_puts_BG96("AT+CIMI\r");
    uart_puts_BG96("AT+QCFG=\"band\",0,0,80,1\r");
    uart_puts_BG96("AT+QCFG=\"iotopmode\",1,1\r");
    uart_puts_BG96("AT+QCFG=\"nwscanseq\",030303,1\r");
    uart_puts_BG96("AT+QCFG=\"gprsattach\",1\r");
    uart_puts_BG96("AT+QCFG=\"nwscanmode\",3,1\r");
    uart_puts_BG96("AT+QCFG=\"nbsibscramble\",1\r");
    uart_puts_BG96("AT+QCFG=\"servicedomain\",2\r");
    uart_puts_BG96("AT+QCFG=\"roamservice\",1\r");
}

void Connect_NB_IOT(void)
{
    uart_puts_BG96("AT+COPS= 1,2,\"20416\",9\r"); //T-Mobile
    uart_puts_BG96("AT+QNWINFO\r");
}

void Transmit_NB_IOT(void)
{

}

/* 2G Functions */
void Init_2G_BG96(void)
{
    uart_puts_BG96("AT+CFUN=0\r");
    Systick_delay(2000);    // delay nodig
    ReceiveCheckQuectel();

   // uart_puts_BG96("AT+CFUN=1\r");
   // Systick_delay(2000);    // delay nodig
   // ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nwscanseq\",010101,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nwscanmode\",1,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"band\",1,80000,80000,1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"roamservice\",1,1\r");
    ReceiveCheckQuectel();

    PowerupCheckBG96(1);  //reset
    Systick_delay(500); //opstart tijd nodig voordat SIM uitgelezen kan worden

    //check setting op goeie waarde toevoegen?
    uart_puts_BG96("AT+QCFG=\"nwscanseq\"\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"nwscanmode\"\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"band\"\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QCFG=\"roamservice\"\r");
    ReceiveCheckQuectel();

    //wanneer goed return 1

    //anders return 0
}

void Connect_2G_BG96(void)
{
    uart_puts_BG96("AT+CFUN=1\r");  //werkt~
    Systick_delay(2000);    // delay nodig
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+COPS=?\r");
    Systick_delay(12500);
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+COPS= 1,2,\"20408\",0\r");
    ReceiveCheckQuectel();
}

void Transmit_2G_BG96(void)
{
    uart_puts_BG96("AT+QHTTPCFG=\"contextid\",1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QICSGP=1,1,\"multimedia.ah.nl\",\"\",\"\",1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QIACT=1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QHTTPCFG=\"sslctxid\",1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QSSLCFG=\"sslversion\",1,3\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QSSLCFG=\"ciphersuite\",1,0x0035\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QSSLCFG=\"seclevel\",1,0\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QHTTPURL=48,80\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("https://top-connector.binary-glue.com/network/1\r");
    ReceiveCheckQuectel();

    uart_puts_BG96("AT+QHTTPPOST= 122,80,80\r");//MESSAGE
    ReceiveCheckQuectel();

    /*
    Network         = 0;    //active network
    GNSS_BG96       = true; //GNSS BG96 on / off
    SignalStrength  = 0;    //Signal Strength in dB
    GPS_Lat         = 0;    //GPS Latitude
    GPS_Lon         = 0;    //GPS Longtitude
    MCC             = 204;  //Mobile Country Code
    MNC             = 8;    //Mobile Network Code
    LAC             = 0;    //Location Area Code
    Cell            = 0;    //Cell information
    Size            = 0;    //test data size in bytes
    Sequence        = 0;    //Test data sequence number
    Samples         = 0;    //Current sample data

*/
    //StringToSend = {Network,GNSS_BG96,SignalStrength,GPS_Lat,GPS_Lon,MCC,MNC,LAC,Cell,Size,Sequence,Samples};


}

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
        PowerupCheckBG96(1);
        Systick_delay(500); //opstart tijd nodig voordat SIM uitgelezen kan worden
    }

    uart_puts_BG96("AT+CIMI\r");

    startup = ReceiveCheckQuectel();

    return startup;
}



#endif /* BG96_H_ */
