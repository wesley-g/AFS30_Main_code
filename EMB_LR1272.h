#ifndef EMB_LR1272_H_
#define EMB_LR1272_H_


/* Embit-LR1272 commands */
void EMB_Dev_Info(void);
void EMB_Dev_State(void);
void EMB_Reset(void);
void EMB_Network_Stop(void);
void EMB_Network_Pref(void);
void EMB_Output_Power(void);
void EMB_Operating_Channel(void);
void EMB_Network_Adres(void);
void EMB_Network_ID(void);
void EMB_Energy_Save_Mode(void);
void EMB_Network_Start(void);
void EMB_Exchange_Data(uint8_t UDPsize);
void EMB_Physical_Address_set(void);
void EMB_Set_AppSKey(void);
void EMB_Set_NwkSKey(void);
uint8_t EMB_Get_Output_Power(void);


/* Embit-LR1272 Functions */
void LoRaEMB(void);
void LoRaEMB_Advanced(void);
void InitLoRaWAN(void);
void ConnectLoRaWAN(void);
void TransmitLoRaWAN(void);


uint8_t status = 0;
uint8_t CRC = 0x69;
uint8_t EMB_length;
uint8_t EMB_Startup(void);


uint8_t EMB_Startup(void)
{
    status = 1; //unsuccesfull startup

    UART_A1_Init(); //EMB UART Init
    EMB_Reset();    //send Reset Command

    if (ReceiveCheckEMB() == 1) // check checksum
    {
        status = 0; //succesfull startup
        return status;
    }
    else
    {
        /*
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4); //HW Reset EMB module
        Systick_delay(1000);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
        */

        EMB_Reset();    //send Reset Command

        if (ReceiveCheckEMB() == 1) // checking the checksum of the answer
        {
            status = 0;
            return status;
        }

        else
        {
            Disable_UART(EUSCI_A1_BASE);

            return status = 1; //unsuccesfull startup
        }
    }
}

uint8_t EMB_Get_Output_Power(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x04);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x10);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x14);
}


void EMB_Dev_Info(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);// terug zetten naar goeie uart
    MAP_UART_transmitData(EUSCI_A1_BASE,0x04);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x01);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x05);
}

void EMB_Dev_State(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x04);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x04);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x08);
}

void EMB_Reset(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x04);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x05);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x09);
}

void EMB_Network_Stop(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x04);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x30);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x34);
}

void EMB_Network_Pref(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x05);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x25);

    // if(LoRa_Mode == LoRaWAN_noJoin)
    // {
        MAP_UART_transmitData(EUSCI_A1_BASE,0xA0);
        MAP_UART_transmitData(EUSCI_A1_BASE,0xCA);
    //}

   /* if(LoRa_Mode == LoRaEMB)
    {
        MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
        MAP_UART_transmitData(EUSCI_A1_BASE,0x2A);
    }*/
}

void EMB_Output_Power(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x05);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x10);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x0A);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x1F);
}

void EMB_Operating_Channel(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x08);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x11);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x01);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x07);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x01);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x22);
}

void EMB_Network_Adres(void)
{
    /* LoRaEMB
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x06);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x21);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x27);
    */
        // LoRaWAN
       MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
       MAP_UART_transmitData(EUSCI_A1_BASE,0x08);
       MAP_UART_transmitData(EUSCI_A1_BASE,0x21);
       MAP_UART_transmitData(EUSCI_A1_BASE,0x26);
       MAP_UART_transmitData(EUSCI_A1_BASE,0x01);
       MAP_UART_transmitData(EUSCI_A1_BASE,0x1A);
       MAP_UART_transmitData(EUSCI_A1_BASE,0xF1);
       MAP_UART_transmitData(EUSCI_A1_BASE,0x5B);


}

void EMB_Network_ID(void)
{
    /* LoRaEMB
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x06);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x22);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x01);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x29);
    */

    //LoRaWAN
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x08);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x22);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x26);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x01);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x1A);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xF1);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x5C);
}

void EMB_Energy_Save_Mode(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x05);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x13);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x18);
}

void EMB_Network_Start(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x04);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x31);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x35);


}

void EMB_Exchange_Data(uint8_t UDPsize)
{
    if(UDPsize % 2 == 0)
    {
        UDPsize = UDPsize / 2;  //LoRa payload = HEX format (2 bytes in 1 HEX)
        EMB_length = 7 + UDPsize;
    }

    else
    {
        UDPsize = UDPsize + 1;
        UDPsize = UDPsize / 2;  //LoRa payload = HEX format (2 bytes in 1 HEX)
        EMB_length = 7 + UDPsize;
    }

     MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
     MAP_UART_transmitData(EUSCI_A1_BASE,EMB_length);
     MAP_UART_transmitData(EUSCI_A1_BASE,0x50);
     MAP_UART_transmitData(EUSCI_A1_BASE,0x0C);
     MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
     MAP_UART_transmitData(EUSCI_A1_BASE,0x06);


     for(s = 0; s < UDPsize; s++)
     {
         MAP_UART_transmitData(EUSCI_A1_BASE,0);
         CRC++;
     }

     MAP_UART_transmitData(EUSCI_A1_BASE,CRC); // CRC CHECK

     /*
     MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
     MAP_UART_transmitData(EUSCI_A1_BASE,0x0B);
     MAP_UART_transmitData(EUSCI_A1_BASE,0x50);
     MAP_UART_transmitData(EUSCI_A1_BASE,0x0C);
     MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
     MAP_UART_transmitData(EUSCI_A1_BASE,0x06);
     MAP_UART_transmitData(EUSCI_A1_BASE,0x01); //CAYENNE CHANNEL
     MAP_UART_transmitData(EUSCI_A1_BASE,0x03); //TYPE OF DATA  (current)
     MAP_UART_transmitData(EUSCI_A1_BASE,0x00); //DATA PART 1
     MAP_UART_transmitData(EUSCI_A1_BASE,0x01); //DATA PART 2
     MAP_UART_transmitData(EUSCI_A1_BASE,0x72); //EBI CRC CHECK
     */
}

void EMB_Physical_Address_set(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x14);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x20);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x70);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xB3);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xD5);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x7E);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xD0);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xFA);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x8E);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x1B);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xC5);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x06);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x70);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x10);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x13);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x07);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x82);
}

void EMB_Set_AppSKey(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x15);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x26);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x11);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x4E);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x6E);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x41);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xB5);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xFC);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xE2);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x42);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xA4);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xAD);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xD5);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x76);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x14);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xAC);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x76);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x60);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x58);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xA8);
}

void EMB_Set_NwkSKey(void)
{
    MAP_UART_transmitData(EUSCI_A1_BASE,0x00);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x15);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x26);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x10);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x3C);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xB2);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x82);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x2B);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x1A);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x01);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xDC);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x82);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xD2);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xE8);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xD2);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x53);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x78);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x57);
    MAP_UART_transmitData(EUSCI_A1_BASE,0xDB);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x7D);
    MAP_UART_transmitData(EUSCI_A1_BASE,0x65);
}

void LoRaEMB(void)
{
    EMB_Network_Stop();
    EMB_Energy_Save_Mode();
    EMB_Network_start();
    EMB_Exchange_Data(Size);
}

void LoRaEMB_Advanced(void)
{
    EMB_Network_Stop();
    while (ReceiveCheckEMB() != 1){}
    EMB_Network_Pref();
    while (ReceiveCheckEMB() != 1){}
    EMB_Output_Power();
    while (ReceiveCheckEMB() != 1){}
    EMB_Operating_Channel();
    while (ReceiveCheckEMB() != 1){}
    EMB_Network_Adres();
    while (ReceiveCheckEMB() != 1){}
    EMB_Network_ID();
    while (ReceiveCheckEMB() != 1){}
    EMB_Energy_Save_Mode();
    while (ReceiveCheckEMB() != 1){}
    EMB_Network_Start();
    while (ReceiveCheckEMB() != 1){}
    EMB_Exchange_Data(Size);
    while (ReceiveCheckEMB() != 1){}
}

void InitLoRaWAN(void)
{
    Network = 0;    //active network
    MCC = 204;  //Mobile Country Code
    MNC = 8;    //Mobile Network Code
    SignalStrength = 0;

    EMB_Network_Stop();
    while (ReceiveCheckEMB() != 1){}
    EMB_Network_Pref();
    while (ReceiveCheckEMB() != 1){}
    EMB_Network_Adres();
    while (ReceiveCheckEMB() != 1){}
    EMB_Network_ID();
    while (ReceiveCheckEMB() != 1){}
    EMB_Physical_Address_set();
    while (ReceiveCheckEMB() != 1){}
    EMB_Set_NwkSKey();
    while (ReceiveCheckEMB() != 1){}
    EMB_Set_AppSKey();
    while (ReceiveCheckEMB() != 1){}
}

void ConnectLoRaWAN(void)
{
    EMB_Network_Start();
    while (ReceiveCheckEMB() != 1){}
}

void TransmitLoRaWAN(void)
{
    EMB_Exchange_Data(Size);


    while (ReceiveCheckEMB() != 1){}
}


#endif /* EMB_LR1272_H_ */
