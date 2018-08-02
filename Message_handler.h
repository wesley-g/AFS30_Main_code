#ifndef MESSAGE_HANDLER_H_
#define MESSAGE_HANDLER_H_

/////////////* function prototypes *////////////////////////////////////
/* UART communication function */
uint8_t uart_putc(unsigned char c);
uint8_t uart_puts_BG96(char *str);

/* UART Messagechecks */
uint8_t ReceiveCheckQuectel(void); //uint8_t *OKBuffer, uint8_t *ERRORBuffer
uint8_t ReceiveCheckEMB(void);
uint8_t ReceiveCheckINP(uint8_t *ENTERBuffer);
uint8_t PowerupCheckBG96(uint8_t Power);
uint8_t PowerupCheckEG91(uint8_t Power);
////////////////////////////////////////////////////////////////////////

#define BUFF_SIZE 150

uint8_t Checksum = 0;
uint8_t Messagefield = 0;
uint8_t Checksumfield = 0;
volatile uint8_t Receive_done = 0; // interne check
volatile uint8_t Powerup_done =0;
volatile uint8_t i =0, x = 0, y = 0 , ii = 0;
volatile uint8_t TXData;
volatile uint8_t RXBuffer[BUFF_SIZE];
volatile uint8_t AT_Error = 0;
volatile uint8_t RDYBuffer[3]= {82,68,89}; // Quectel RDY Buffer
volatile uint8_t OKBuffer[4] = {79, 75, 13, 10 }; //Quectel  OK,/x0d,/x0a
volatile uint8_t ERRORBuffer[7] = {69, 82, 82, 79, 82, 13, 10 }; //Quectel ERROR/x0d,/x0a
//volatile uint8_t ENTERBuffer[26] = {69, 78, 84, 69, 82, 32, 67, 79, 77, 77, 65,78, 68, 32, 84, 69, 83, 84, 32, 77, 79, 68, 69, 10, 13, 10, 13 };
                                           //Innophase ENTER COMMAND TEST MODE,\x0a,\x0d,\x0a,\x0d,#

uint8_t uart_puts_BG96(char *str) // Sends a String to the UART.
{
    Receive_done = 0;
    AT_Error = 0;

    while (*str)
    {
        MAP_UART_transmitData(EUSCI_A0_BASE, (char *) *str++);
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

uint8_t PowerupCheckBG96(uint8_t Power)
{
    Powerup_done = 0;

    if (Power == 0)
    {
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN3); // Power BG96 high
        Systick_delay(200);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN3); //Power BG96 low
    }


    if (Power == 1)
    {
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN2); // reset BG96 high
        Systick_delay(200);
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
        Systick_delay(200);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5); // Power EG91 high
    }

    if (Power == 1)
    {
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6); // reset EG91 high
        Systick_delay(200);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN6); // reset EG91 high
    }

    while (Powerup_done != 1)
    {
            for (x = 0; x < 15; x++)
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
//////////////////////////end of Quectel message check

uint8_t ReceiveCheckEMB(void)
{
    Receive_done = 0;
    Checksum = 0;
    Messagefield = 0;

    while (Receive_done != 1)
    {
        for (x = 0; x < 10; x++)
        {
            if (RXBuffer[x] == 0x00 && RXBuffer[x+1] != 0x00) // check voor 0x00
            {
                x++;
                Messagefield = RXBuffer[x] -1;
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
        } // einde for loop
    } // einde while loop
} //einde functie
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



#endif /* MESSAGE_HANDLER_H_ */
