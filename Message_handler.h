#ifndef MESSAGE_HANDLER_H_
#define MESSAGE_HANDLER_H_

#define BUFF_SIZE 200

/////////////* function prototypes *////////////////////////////////////
/* UART communication function */
uint8_t uart_putc(unsigned char c);
uint8_t uart_puts_BG96(char *str);
void Create_Message(void);
void Build_message(void);

/* UART Messagechecks */
uint8_t ReceiveCheckQuectel(void); //uint8_t *OKBuffer, uint8_t *ERRORBuffer
uint8_t ReceiveCheckEMB(void);
uint8_t ReceiveCheckINP(uint8_t *ENTERBuffer);
uint8_t PowerupCheckBG96(uint8_t Power);
uint8_t PowerupCheckEG91(uint8_t Power);
uint8_t ConnectCheckQuectel(uint8_t GPS);
uint8_t QuectelOkCheck(void);
uint8_t QuectelCSQCheck(void);
////////////////////////////////////////////////////////////////////////

uint8_t Checksum = 0;
uint8_t Messagefield = 0;
uint8_t Checksumfield = 0;
volatile uint8_t Receive_done = 0; // interne check
volatile uint8_t Powerup_done = 0;
volatile uint8_t i = 0, x = 0, y = 0 , ii = 0, counter = 0;
volatile uint8_t TXData;
volatile uint8_t RXBuffer[BUFF_SIZE];
volatile uint8_t AT_Error = 0;
volatile uint8_t RDYBuffer[3]= {82,68,89}; // Quectel RDY Buffer
volatile uint8_t OKBuffer[4] = {79, 75, 13, 10 }; //Quectel  OK,/x0d,/x0a
volatile uint8_t ERRORBuffer[7] = {69, 82, 82, 79, 82, 13, 10 }; //Quectel ERROR/x0d,/x0a
extern int message2 = 1;
int IndexCounter = 0;

/* Variables to cloud */
int Network;       //active network
bool GNSS_BG96;      //GNSS BG96 on / off
int SignalStrength;  //Signal Strength in dB
float GPS_Lat;     //GPS Latitude
float GPS_Lon;      //GPS Longtitude
int MCC;      //Mobile Country Code
int MNC;       //Mobile Network Code
int LAC;      //Location Area Code
int Cell;      //Cell information
int Size;      //test data size in bytes
int Sequence;     //Test data sequence number
int m_characters;

static volatile uint_fast16_t Samples[800];       //Current sample data
char StringToSend[5000];
int s,r;
int GPS_Count = 0;
char tempchar[8];
char tempchar1[8];

uint8_t uart_puts_BG96(char *str) // Sends a String to the UART.
{
    Receive_done = 0;
    AT_Error = 0;

    while (*str)
    {
        MAP_UART_transmitData(EUSCI_A0_BASE, (char *) *str++);
    }
}

uint8_t uart_puts_EMB(char *str) // Sends a String to the UART.
{
    Receive_done = 0;
    AT_Error = 0;

    while (*str)
    {
        MAP_UART_transmitData(EUSCI_A1_BASE, (char *) *str++);
    }
}

uint8_t uart_puts_EG91(char *str) //Sends a String to the UART.
{
    Receive_done = 0;
    AT_Error = 0;

    while (*str)
    {
        MAP_UART_transmitData(EUSCI_A3_BASE, (char *) *str++);
    }
}

void Build_message(void)
{
    sprintf(StringToSend,
            "{\"network\":%d,\"bg96\":%d,\"signal\":%d,\"lat\":%f,\"lon\":%f,\"mcc\":%d,\"mnc\":%d,\"lac\":%d,\"cid\":%d,\"size\":%d,\"sequence\":%d,\"samples\":[%d",
            Network, GNSS_BG96, SignalStrength, GPS_Lat, GPS_Lon, MCC, MNC, LAC, Cell, Size, Sequence, Samples[0]);

    for(s = 0; s < MovAvgTransmissionIndex; s++)
    {
        sprintf(StringToSend,"%s,%d",StringToSend,Samples[s]);
    }

    sprintf(StringToSend,"%s]}/r",StringToSend);
}

void Message_counter(void)
{
     m_characters = MovAvgIdleIndex + MovAvgInitialisingIndex + MovAvgConnectedIndex + MovAvgTransmissionIndex;

    if (m_characters >= 800)
    {
        if (m_characters >= 1600)
        {
            if (m_characters >= 2400)
            {
                if (m_characters >= 3200)
                {

                }
                else
                {
                    message2 = 4;
                }
            }
            else
            {
                message2 = 3;
            }
        }
        else
        {
            message2 = 2;
        }
    }
    else
    {
        message2 = 1;
    }
}

void Create_Message(void)
{
    if(GPS_Lat != 0 && GPS_Lon != 0)
    {
        GNSS_BG96 = true;
    }

    else
    {
        GNSS_BG96 = false;
    }

    LAC = 0;    //Location Area Code
    Cell = 0;    //Cell information

    if(IndexCounter == 4)
    {
        IndexCounter = 1;
    }

    if(IndexCounter == 5)
    {
        IndexCounter = 2;
    }

    if(IndexCounter == 6)
    {
        IndexCounter = 3;
    }

    //////////////////////////////////
    // Idle = value                 //
    // Init = 16383 + value         //
    // Connect = 32767 + value      //
    // Transmissie = 49151 + value  //
    //////////////////////////////////////////////////////////////////////////
    if (IndexCounter == 0)
    {
        for (s = 0; s < MovAvgIdleIndex; s++)
        {
            Samples[s] = MovAvgIdle[s];
        }

        MovAvgInitialisingIndex = MovAvgInitialisingIndex + MovAvgIdleIndex;

        if (MovAvgInitialisingIndex >= 800)
        {
            Build_message();
            MovAvgInitialisingIndex = 0;
            IndexCounter = 4; //niet doorgaan
        }

        else
        {
            IndexCounter = 1;
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    if (IndexCounter == 1)
    {

        for (s = MovAvgIdleIndex; s < MovAvgInitialisingIndex; s++)
        {
            Samples[s] = MovAvgInitialising[s - MovAvgIdleIndex] + 16384;
        }

        MovAvgConnectedIndex = MovAvgConnectedIndex + MovAvgInitialisingIndex;

        if (MovAvgConnectedIndex >= 800)
        {
            Build_message();
            MovAvgConnectedIndex = 0;
            IndexCounter = 5; //niet doorgaan
        }

        else
        {
            IndexCounter = 2;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    if (IndexCounter == 2)
    {

        for (s = MovAvgInitialisingIndex; s < MovAvgConnectedIndex; s++)
        {
            Samples[s] = MovAvgConnected[s - MovAvgInitialisingIndex] + 32768;
        }

        MovAvgTransmissionIndex = MovAvgTransmissionIndex
                + MovAvgConnectedIndex;

        if (MovAvgTransmissionIndex >= 800)
        {
            Build_message();
            MovAvgTransmissionIndex = 0;
            IndexCounter = 6; //niet doorgaan
        }

        else
        {
            IndexCounter = 3;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////
    if (IndexCounter == 3)
    {

        for (s = MovAvgConnectedIndex; s < MovAvgTransmissionIndex; s++)
        {
            Samples[s] = MovAvgTransmission[s - MovAvgConnectedIndex] + 49152;
        }

        Build_message();
        IndexCounter = 0; // reset voor volgende bericht
    }
}
////////////////////////////////////////////////////////////////////////////////////////

uint8_t PSMcheck(void)
{
    Powerup_done = 0;

    while (Powerup_done != 1)
    {
        for (x = 0; x < 40; x++)
        {
            if (RXBuffer[x] == 0x50 && RXBuffer[x + 1] == 0x4F
                    && RXBuffer[x + 2] == 0x57 && RXBuffer[x + 3] == 0x45 && RXBuffer[x + 4] == 0x52) // POWER DOWN
            {
                for (x = 0; x < BUFF_SIZE; x++)
                {
                    RXBuffer[x] = 0;
                }

                return Powerup_done = 1;
            }
        }
    }
}


uint8_t PowerupCheckBG96(uint8_t Power)
{
    Powerup_done = 0;

    if (Power == 0)
    {
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN3); // Power BG96 high
        Systick_delay(1000);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN3); //Power BG96 low
    }


    if (Power == 1)
    {
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN2); // reset BG96 high
        Systick_delay(500);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN2); //Power BG96 low
    }

    while (Powerup_done != 1)
    {
        for (x = 0; x < 10; x++)
        {

            if (RXBuffer[x] == 0x52 && RXBuffer[x + 1] == 0x44 && RXBuffer[x + 2] == 0x59) // R D Y
            {
                for (x = 0; x < BUFF_SIZE; x++)
                {
                    RXBuffer[x] = 0;
                }

                return Powerup_done = 1;
            }
            else
            {
                uart_puts_BG96("AT\r");

                if (ReceiveCheckQuectel() == 0)
                {
                    return Powerup_done = 1;
                }
            }
        }
    }
}

uint8_t PowerupCheckEG91(uint8_t Power) // if power == 0 Power // if power == 1 Reset
{
    Powerup_done = 0;

    if (Power == 0)
    {
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5); // Power EG91 high
        Systick_delay(1000);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5); // Power EG91 high
    }

    if (Power == 1)
    {
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6); // reset EG91 high
        Systick_delay(500);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN6); // reset EG91 high
    }

    while (Powerup_done != 1)
    {
        for (x = 0; x < 10; x++)
        {
            if (RXBuffer[x] == 0x52 && RXBuffer[x + 1] == 0x44
                    && RXBuffer[x + 2] == 0x59) // R D Y
            {
                for (x = 0; x < BUFF_SIZE; x++)
                {
                    RXBuffer[x] = 0;
                }

                return Powerup_done = 1;
            }
            else
            {
                uart_puts_EG91("AT\r");

                if (ReceiveCheckQuectel() == 0)
                {
                    return Powerup_done = 1;
                }
            }
        }
    }
}

    // Check for ERROR / OK
    // ERROR =1
    // OK = 0
    // eerdere Quectelcheck te gebruiken?  Buffer niet leegmaken!!!!!

    // als Quectelcheck = 0 (OK)
    // zoeken naar Lat Lon waarden

uint8_t ConnectCheckQuectel(uint8_t GPS)
{
    while (Receive_done != 1)
    {
        for (x = 0; x < BUFF_SIZE; x++)
        {
            if (RXBuffer[x] == 0x43) //check 1 connect
            {
                if (RXBuffer[x + 1] == 0x54)
                {
                    if (RXBuffer[x + 2] == 0x0D)
                    {
                        if (RXBuffer[x + 3] == 0x0A)
                        {
                            i = 0;
                            Receive_done = 0;

                            for (x = 0; x < BUFF_SIZE; x++)
                            {
                                RXBuffer[x] = 0;
                            }

                            return AT_Error = 0;

                        }
                    }
                }
            } // end OKBuffer check

            else if (RXBuffer[x] == 0x4F) //check 1 OK
            {
                if (RXBuffer[x + 1] == 0x4B)
                {
                    if (RXBuffer[x + 2] == 0x0D)
                    {
                        if (RXBuffer[x + 3] == 0x0A)
                        {
                            i = 0;
                            Receive_done = 0;
////////////////////////////////////////////////////////////////////

                            /////GPS LAT LON CHECK///////

                            if (GPS == 0)
                            {
                                for (x = 0; x < BUFF_SIZE; x++)
                                {
                                    RXBuffer[x] = 0;
                                }

                                return AT_Error = 0;
                            }

                            else if (GPS == 1)
                            {
                                for (x = 0; x < 200; x++)
                                {
                                    if (RXBuffer[x] == 0x2C && GPS_Count == 1)
                                    {
                                        sprintf(tempchar, "%c.%c%c%c%c%c",
                                                RXBuffer[x + 1],
                                                RXBuffer[x + 3],
                                                RXBuffer[x + 4],
                                                RXBuffer[x + 5],
                                                RXBuffer[x + 6],
                                                RXBuffer[x + 7]);

                                        GPS_Lon = atof(tempchar);

                                        for (r = 0; r < 9; r++)
                                        {
                                            tempchar[r] = 0;
                                        }

                                        GPS_Count++;

                                    }
                                    if (RXBuffer[x] == 0x2C && GPS_Count == 0)
                                    {
                                        sprintf(tempchar, "%c%c.%c%c%c%c%c",
                                                RXBuffer[x + 1],
                                                RXBuffer[x + 2],
                                                RXBuffer[x + 4],
                                                RXBuffer[x + 5],
                                                RXBuffer[x + 6],
                                                RXBuffer[x + 7],
                                                RXBuffer[x + 8]);

                                        GPS_Lat = atof(tempchar);

                                        for (r = 0; r < 9; r++)
                                        {
                                            tempchar[r] = 0;
                                        }

                                        GPS_Count++;
                                    }
                                }

                                GPS_Count = 0;

//////////////////////////////////////////////////////////////////////////

                                for (x = 0; x < BUFF_SIZE; x++)
                                {
                                    RXBuffer[x] = 0;
                                }

                                return AT_Error = 0;

                            }
                        }
                    }
                } // end OKBuffer check
            }

            else if (RXBuffer[x] == 0x45) //check 1 ERROR
            {
                if (RXBuffer[x + 1] == 0x52)
                {
                    if (RXBuffer[x + 2] == 0x52)
                    {
                        if (RXBuffer[x + 3] == 0x4F)
                        {
                            if (RXBuffer[x + 4] == 0x52)
                            {
                                {
                                    i = 0;
                                    Receive_done = 0;

                                    for (x = 0; x < BUFF_SIZE; x++)
                                    {
                                        RXBuffer[x] = 0;
                                    }

                                    return AT_Error = 1; //error
                                }
                            }
                        }
                    }
                }
            }
        }   // end ERRORBuffer check
    }
}


uint8_t QuectelCSQCheck(void)
{
    while (Receive_done != 1)
    {
        for (ii = 0; ii < 40; ii++)
        {
            for (x = 0; x < BUFF_SIZE; x++)
            {
                if (RXBuffer[x] == 0x3A) //check 1 connect
                {
                    if (RXBuffer[x + 1] == 0x20)
                    {
                        if (RXBuffer[x + 11] == 0x4F)
                        {
                            if (RXBuffer[x + 12] == 0x4B)
                            {

                                SignalStrength = ((RXBuffer[x + 2] - 48) * 10)
                                        + ((RXBuffer[x + 3] - 48));

                                if (SignalStrength == 99)
                                {
                                    SignalStrength = 0; // geen bereik of niet detecteerbaar
                                }
                                else
                                {
                                    SignalStrength = -113
                                            + (SignalStrength * 2); //RSSI berekening
                                }

                                i = 0;
                                Receive_done = 0;

                                Systick_delay(500);

                                for (x = 0; x < BUFF_SIZE; x++)
                                {
                                    RXBuffer[x] = 0;
                                }

                                return AT_Error = 0;
                            }
                        }
                    }
                } // end OKBuffer check

                else if (RXBuffer[x] == 0x45) //check 1 ERROR
                {
                    if (RXBuffer[x + 1] == 0x52)
                    {
                        if (RXBuffer[x + 2] == 0x52)
                        {
                            if (RXBuffer[x + 3] == 0x4F)
                            {
                                if (RXBuffer[x + 4] == 0x52)
                                {
                                    {
                                        SignalStrength = 0;

                                        i = 0;
                                        Receive_done = 0;

                                        for (x = 0; x < BUFF_SIZE; x++)
                                        {
                                            RXBuffer[x] = 0;
                                        }

                                        return AT_Error = 1; //error
                                    }
                                }
                            }
                        }
                    }
                }
            }   // end ERRORBuffer check

            if (ii == 39)
            {
                Receive_done = 0; //timeout
                i = 0;

                for (x = 0; x < BUFF_SIZE; x++)
                {
                    RXBuffer[x] = 0;
                }

                return AT_Error = 1;
            }
        }
    }
}

uint8_t ReceiveCheckQuectel(void)
{
    while (Receive_done != 1)
    {
        for (ii = 0; ii < 40; ii++)
        {
            for (x = 0; x < BUFF_SIZE; x++)
            {
                if (RXBuffer[x] == 0x4F) //check 1 OK
                {
                    if (RXBuffer[x + 1] == 0x4B)
                    {
                        if (RXBuffer[x + 2] == 0x0D)
                        {
                            if (RXBuffer[x + 3] == 0x0A)
                            {
                                i = 0;
                                Receive_done = 0;

                                for (x = 0; x < BUFF_SIZE; x++)
                                {
                                    RXBuffer[x] = 0;
                                }

                                return AT_Error = 0;
                            }
                        }
                    }
                } // end OKBuffer check

                else if (RXBuffer[x] == 0x45) //check 1 ERROR
                {
                    if (RXBuffer[x + 1] == 0x52)
                    {
                        if (RXBuffer[x + 2] == 0x52)
                        {
                            if (RXBuffer[x + 3] == 0x4F)
                            {
                                if (RXBuffer[x + 4] == 0x52)
                                {
                                    if (RXBuffer[x + 5] == 0x0D)
                                    {
                                        if (RXBuffer[x + 6] == 0x0A)
                                        {
                                            i = 0;
                                            Receive_done = 0;

                                            for (x = 0; x < BUFF_SIZE; x++)
                                            {
                                                RXBuffer[x] = 0;
                                            }

                                            return AT_Error = 1; //error
                                        }
                                    }
                                }
                            }
                        }
                    }
                }   // end ERRORBuffer check
            }  //end of for loop for checking Quectel messages

            if(ii == 39)
            {
                Receive_done = 0; //timeout
                i = 0;

                for (x = 0; x < BUFF_SIZE; x++)
                {
                    RXBuffer[x] = 0;
                }

                return AT_Error = 1;
            }
        }  // end while loop for Quectel message check
    }
}

uint8_t QuectelOkCheck(void)
{
    while (Receive_done != 1)
    {
        for (ii = 0; ii < 40; ii++)
        {
            for (x = 0; x < BUFF_SIZE; x++)
            {
                if (RXBuffer[x] == 0x0A) //check 1 OK
                {
                    if (RXBuffer[x + 1] == 0x0D)
                    {
                        if (RXBuffer[x + 2] == 0x0A)
                        {
                            if (RXBuffer[x + 3] == 0x4F)
                            {
                                if (RXBuffer[x + 4] == 0x4B)
                                {
                                    i = 0;
                                    Receive_done = 0;

                                    for (x = 0; x < BUFF_SIZE; x++)
                                    {
                                        RXBuffer[x] = 0;
                                    }

                                    return AT_Error = 0;
                                }
                            }
                        }
                    } // end OKBuffer check
                }

                if (ii == 39) //timeout function
                {
                    Receive_done = 0; //timeout
                    i = 0;

                    for (x = 0; x < BUFF_SIZE; x++)
                    {
                        RXBuffer[x] = 0;
                    }

                    return AT_Error = 1;
                }

            }
        }
    }
}
//////////////////////////end of Quectel message check

uint8_t ReceiveCheckEMB(void)
{
    Receive_done = 0;
    Checksum = 0;
    Messagefield = 0;

    while (Receive_done != 1)
    {
        for (ii = 0; ii < 40; ii++)
        {
            for (x = 0; x < 15; x++)
            {
                if (RXBuffer[x] == 0x00 && RXBuffer[x + 1] != 0x00) // check voor 0x00
                {
                    x++;
                    Messagefield = RXBuffer[x] - 1;
                    // Checksum = RXBuffer[x]; //messagefield meenemen voor Checksum berekening
                    if (RXBuffer[Messagefield] != 0) //goede plek in buffer gevonden voor lengte
                    {
                        Checksumfield = RXBuffer[x] - 1; // checksum niet meenemen
                        x--;

                        for (y = 0; y < Checksumfield; y++) //Checksum berekening
                        {
                            Checksum = Checksum + RXBuffer[x + y];
                        }  //Checksum berekend

                        if (Checksum == RXBuffer[Checksumfield]) // de checksum bytes
                        {
                            Receive_done = 1;
                            i = 0;

                            for (x = 0; x < BUFF_SIZE; x++)
                            {
                                RXBuffer[x] = 0;
                            }

                            return Receive_done;
                        }
                        else
                        {
                            Checksum = 0;
                        }
                    }
                }
/*
                if (ii == 39) //timeout function
                {
                    Receive_done = 0; //timeout
                    i = 0;

                    for (x = 0; x < BUFF_SIZE; x++)
                    {
                        RXBuffer[x] = 0;
                    }

                    for (j = 0; j < 15; j ++)   //MCU LED on; j++)
                    {
                        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                        Systick_delay(100);
                    }

                    return Receive_done;
                }*/
            } // einde for loop
        } // einde while loop
    } //einde functie
}
/*
uint8_t ReceiveCheckINP(uint8_t *ENTERBuffer)
{
    while (Receive_done != 1)
    {
        for (x = 0; x < BUFF_SIZE; x++)
        {
            if (RXBuffer[x] == ENTERBuffer[0]) //check 1
            {
                for (y = 0; y < 26; y++)
                {
                    if (RXBuffer[x] != ENTERBuffer[y])
                    {
                        return AT_Error = 1;
                    }

                    x++;
                }

                i = 0;
                Receive_done = 1;
                return AT_Error = 0;
            } // end OKBuffer check
        }
    }
}*/

/*
 * command = 0 AT+QCSQ
 * command = 1 AT+CBC
 * command = 2 AT+QTEMP
 */

int rssi,rsrp,sinr,rsrq;
int bcl,vbat;
int pmic,xo,pa;

void DataCheck(int command)
{
    int done,start,end,datacount;

    while (Receive_done != 1)
    {
        for(x = 0; x < BUFF_SIZE; x++)
        {
            if(RXBuffer[x] == 0x4F) // OK check
            {
                if(RXBuffer[x + 1] == 0x4B)
                {
                    if(command == 0)
                    {
                        x = 0;

                        while(RXBuffer[x] != 0x2C)//begin data opzoeken (komma)
                        {
                            x++;
                        }

                        start = x;
                        done = 0;
                        datacount = 0;

                        x++;//skip current ","

                        while(done == 0)
                        {
                            while(RXBuffer[x] != 0x0D && RXBuffer[x] != 0x2C) //einde waarde/data opzoeken
                            {
                                x++;
                            }

                            end = x;

                            for(x = start + 1; x < end; x++) //paste array into char
                            {
                                sprintf(tempchar,"%s%c",tempchar,RXBuffer[x]);
                            }

                            if(datacount == 0)
                            {
                                rssi = atoi(tempchar);
                            }

                            else if(datacount == 1)
                            {
                                rsrp = atoi(tempchar);
                            }

                            else if(datacount == 2)
                            {
                                sinr = atoi(tempchar);
                            }

                            else if(datacount == 3)
                            {
                                rsrq = atoi(tempchar);
                            }

                            for (r = 0; r < 9; r++) // clean tempchar
                            {
                                tempchar[r] = 0;

                            }

                            if(RXBuffer[end] == 0x2C)//als komma -> nog een waarde
                            {
                                start = end;
                                x = end + 1;
                                datacount++;
                            }
                            else
                            {
                                done = 1;
                                Receive_done = 1;

                                for (x = 0; x < BUFF_SIZE; x++)
                                {
                                    RXBuffer[x] = 0;
                                }

                            }
                        }
                    }

                    else if(command == 1)
                    {
                        x = 0;

                        while(RXBuffer[x] != 0x2C)//begin data opzoeken (komma)
                        {
                            x++;
                        }

                        start = x;
                        done = 0;
                        datacount = 0;

                        x++;//skip current ","

                        while (done == 0)
                        {
                            while (RXBuffer[x] != 0x0D && RXBuffer[x] != 0x2C) //einde waarde/data opzoeken
                            {
                                x++;
                            }

                            end = x;

                            for (x = start + 1; x < end; x++) //paste array into char
                            {
                                sprintf(tempchar,"%s%c", tempchar,RXBuffer[x]);
                            }

                            if (datacount == 0)
                            {
                                bcl = atoi(tempchar);
                            }

                            else if (datacount == 1)
                            {
                                vbat = atoi(tempchar);
                            }

                            for (r = 0; r < 9; r++) // clean tempchar
                            {
                                tempchar[r] = 0;
                            }

                            if (RXBuffer[end] == 0x2C) //als komma -> nog een waarde
                            {
                                start = end;
                                x = end + 1;
                                datacount++;
                            }
                            else
                            {
                                done = 1;
                                Receive_done = 1;

                                for (x = 0; x < BUFF_SIZE; x++)
                                {
                                    RXBuffer[x] = 0;
                                }

                            }
                        }

                    }

                    else if(command == 2)
                    {
                        x = 0;

                        while(RXBuffer[x] != 0x20)//begin data opzoeken (spatie)
                        {
                            x++;
                        }

                        start = x;
                        done = 0;
                        datacount = 0;

                        x++;//skip current ","


                        while(done == 0)
                        {
                            while(RXBuffer[x] != 0x0D && RXBuffer[x] != 0x2C) //einde waarde/data opzoeken
                            {
                                x++;
                            }

                            end = x;

                            for(x = start + 1; x < end; x++) //paste array into char
                            {
                                sprintf(tempchar,"%s%c",tempchar,RXBuffer[x]);
                            }

                            if(datacount == 0)
                            {
                                pmic = atoi(tempchar);
                            }

                            else if(datacount == 1)
                            {
                                xo = atoi(tempchar);
                            }

                            else if(datacount == 2)
                            {
                                pa = atoi(tempchar);
                            }

                            for (r = 0; r < 9; r++) // clean tempchar
                            {
                                tempchar[r] = 0;

                            }

                            if(RXBuffer[end] == 0x2C)//als komma -> nog een waarde
                            {
                                start = end;
                                x = end + 1;
                                datacount++;
                            }
                            else
                            {
                                done = 1;
                                Receive_done = 1;

                                for (x = 0; x < BUFF_SIZE; x++)
                                {
                                    RXBuffer[x] = 0;
                                }

                            }
                        }
                    }
                }
            }
        }
    }
}

#endif /* MESSAGE_HANDLER_H_ */
