#ifndef GPIO_H_
#define GPIO_H_

void MCU_led(void);
void Init_Buttons(void);
void Check_Buttons(void);

volatile uint8_t j;
uint8_t jj;
extern volatile uint8_t Module_status = 0;

void MCU_led(void)
{
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN6);       //MCU LED init// port 1 pin 0
}

void Init_Buttons(void)
{

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN3); //BG96 Power
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN2); //BG96 Reset
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5); //EG91 Power
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6); //EG91 Reset
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4); //EMB  Reset
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN2); //WIFI  Reset

    /* Configuring GPIO as output and P7.0, P4.4, P2.5 (Chip Select) as input */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0); //MCU CS BG96
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4); //MCU CS EG91
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN5); //MCU CS EMB-LR1272

    /* Configuring P8.0 , P7.7, P7.5, P7.4 as an input */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P8, GPIO_PIN0); //NB_IOT Button
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P7, GPIO_PIN7); //CAT_M  Button
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P7, GPIO_PIN5); //4G_LTE Button
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P7, GPIO_PIN4); //WIFI   Button
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P7, GPIO_PIN6); //LORA   Button

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN3);     //Power BG96 Low
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN2);     //Reset BG96 Low
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);     //Power EG91 Low
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN6);     //Reset EG91 Low
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);     //Reset EMB Low
}

void Check_Buttons(void)
{
    /*-------------------------MCU LED on--------------------------*/
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN6);   //MCU LED on PORT 8 PIN 6


    /*-------------------WIFI Button Sequence----------------------*//*
    if (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN4) == 0) //WIFI button
    {
        while (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN4) == 0)
        {
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED off

            //UART_A2_Init(); //INP6000 UART Init

            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);

            // verfiy UART working
            // if yes continue else HW reset
        }

        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
    }*/


    /*-------------------4G LTE Button Sequence---------------------*/
    if (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN5) == 0) // 4G_LTE button
    {
        while (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN5) == 0)
        {
            Module_status = 1; //van startup naar idle (NC)
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);  //MCU LED off
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4); //ADC Enable HIGH EG91 A1

            Init_ADC_EG91();

            if(EG91_Startup() == 0)//succesful and SIM detected
            {
                Systick_delay(1000); // om idle (NC) te meten
                Module_status = 2; //van idle (NC) tot idle (C)
                Init_4G();

                Module_status = 3; //idle (C)
                Connect_4G();

                Module_status = 4; //van idle (C) naar transmissie naar idle (C)
                Transmit_4G();

                Systick_delay(15000);
            }

            else //failed
            {
                for(j = 0; j<15; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }
            }
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4); //ADC Enable HIGH EG91 A1
        }
    }


    /*-------------------LORA Button Sequence----------------------*/
    if (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN6) == 0) //LORA button
    {
        while (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN6) == 0)
        {
            Module_status = 1; //van startup naar idle (NC)
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);   //MCU LED low
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5);  //ADC Enable HIGH EMB-LR1272 A3

            Init_ADC_EMB();

            if (EMB_Startup() == 0)
            {
                Module_status = 2; //van idle (NC) tot idle (C)
                InitLoRaWAN();

                Module_status = 3; //idle (C)
                ConnectLoRaWAN();

                Module_status = 4; //van idle (C) naar transmissie naar idle (C)
                TransmitLoRaWAN();
            }

            else //failed
            {
                for (j = 0; j < 15; j ++)   //MCU LED on; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }
            }
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);  //ADC Enable HIGH EMB-LR1272 A3
        }
    }


    /*-------------------CAT-M Button Sequence----------------------*/

    if (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN7) == 0) //CAT_M button
    {
        while (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN7) == 0)
        {
            Module_status = 1; //van startup naar idle (NC)
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);  //MCU LED low
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable HIGH BG96 A0

            Init_ADC_BG96();

            if(BG96_Startup() == 0)
            {
                Systick_delay(2000); // to get to idle

                Module_status = 2; //van idle (NC) tot idle (C)
                Init_2G_BG96();
                //Init_CAT_M();

                Module_status = 3; //van idle (NC) tot idle (C)
                Connect_2G_BG96();
                //Connect_CAT_M();

                Module_status = 4;
                //Transmit_CAT_M();
                Transmit_2G_BG96();
                Systick_delay(15000);

               // Init_CAT_M();

               // Module_status = 3;
               // Connect_CAT_M();

               // Module_status = 4;
               // Transmit_CAT_M();
             }

            else//failed
            {
                for(j=0; j<15; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }
            }
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable HIGH BG96 A0
        }
    }


    /*-------------------NB_IOT Button Sequence----------------------*/

    if (MAP_GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN0) == 0) //NB_IOT button
    {
        while (MAP_GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN0) == 0)
        {
            Module_status = 1; //van startup naar idle (NC)
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);  //MCU LED low
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable HIGH BG96 A0

            Init_ADC_BG96();

            if(BG96_Startup() == 0)
            {
                Systick_delay(1000);

                Module_status = 2; //van idle (NC) tot idle (C)
                Init_NB_IOT();

                Module_status = 3; //van idle (NC) tot idle (C)
                Connect_NB_IOT();

                Module_status = 4; //van idle (NC) tot idle (C)
                Transmit_NB_IOT();
            }

            else//failed
            {
                for(j=0; j<15; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }
            }
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable HIGH BG96 A0
        }
    }
}

#endif /* GPIO_H_ */
