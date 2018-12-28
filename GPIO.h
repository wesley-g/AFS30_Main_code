#ifndef GPIO_H_
#define GPIO_H_

void MCU_led(void);
void Init_Buttons(void);
void Check_Buttons(void);

volatile uint8_t j;
uint8_t jj;
volatile int Module_status = 0;

int rssi,rsrp,sinr,rsrq;
int bcl,vbat;
int pmic,xo,pa;

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


    /*-------------------WIFI Button Sequence----------------------*/
    if (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN4) == 0) //WIFI button
    {
        while (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN4) == 0)
        {

            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);  //MCU LED low

            Init_ADC_BG96();

            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable HIGH BG96 A0

            Systick_delay(500);
            Module_status = 1; //van startup naar idle (NC)

            if (BG96_Startup() == 0)
            {

                //Init_CAT_M();
                //Connect_2G_BG96();
                Init_CAT_M();
                Connect_CAT_M();

                //Init_NB_IOT();
                //Connect_NB_IOT();

                connect_MyCayenne();

                uart_puts_BG96("AT+QCSQ\r");
                Systick_delay(100);
                DataCheck(0);
                //extract rssi,rsrp,sinr,rsrq

                uart_puts_BG96("AT+CBC\r");
                Systick_delay(100);
                DataCheck(1);
                //extract voltage

                uart_puts_BG96("AT+QTEMP\r");
                Systick_delay(100);
                DataCheck(2);
                //extract pmic,xo,pa temp

                //Module_status = 2; //van startup naar idle (NC)

                send_MyCayenne(0,pmic,1); //channel,data,constant
                send_MyCayenne(1,xo,1);

                //Module_status = 3; //van startup naar idle (NC)

                send_MyCayenne(2,pa,1);
                send_MyCayenne(3,vbat,2);
                send_MyCayenne(4,rssi,3);
                send_MyCayenne(5,rsrp,3);


                //Module_status = 4; //van startup naar idle (NC)

                send_MyCayenne(6,sinr,4);
                send_MyCayenne(7,rsrq,4);
                send_MyCayenne(8,bcl,5);

                close_MyCayenne();

/*
                uart_puts_BG96("AT+QSCLK=0\r"); //DEEP SLEEP
                Systick_delay(100);
                ReceiveCheckQuectel();

                uart_puts_BG96("AT+CPSMS=0\r"); //PSM UIT
                Systick_delay(100);
                ReceiveCheckQuectel();

                Init_NB_IOT();*/
/*
                uart_puts_BG96("AT+QCFG=\"psm/enter\",1\r");
                Systick_delay(100);
                ReceiveCheckQuectel();
*/
  /*              uart_puts_BG96("AT+CEDRXS=1,5,\"0010\"\r"); //EDRX
                Systick_delay(100);
                ReceiveCheckQuectel();

                Module_status = 2;

                uart_puts_BG96("AT+COPS=1,2,\"20404\",9\r"); //Vodafone NB-IoT
                Systick_delay(2000);
                ReceiveCheckQuectel();

                uart_puts_BG96("AT+CPSMS=1,,,\"10001010\",\"00000001\"\r");
                Systick_delay(100);
                ReceiveCheckQuectel();

                uart_puts_BG96("AT+CSQ\r");
                Systick_delay(100);
                QuectelCSQCheck();

                uart_puts_BG96("AT+QNWINFO\r");
                Systick_delay(100);
                ReceiveCheckQuectel();

                Module_status = 3;

                uart_puts_BG96("AT+QPING=1,\"8.8.8.8\",15,4\r");
                Systick_delay(3000);
                ReceiveCheckQuectel();

                uart_puts_BG96("AT+QSCLK=1\r"); //DEEP SLEEP
                Systick_delay(100);
                ReceiveCheckQuectel();

                Disable_UART(EUSCI_A0_BASE);

                Module_status = 4;

                Systick_delay(10000);*/
            }

            else //failed
            {
                for (j = 0; j < 15; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }
            }

            //MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable Low BG96 A0
            //Disable_ADC();
            //Reset_Index();
            //Disable_UART(EUSCI_A1_BASE);
        }
    }

    /*-------------------4G LTE Button Sequence---------------------*/
    if (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN5) == 0) // 4G_LTE button
    {
        while (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN5) == 0)
        {
            Module_status = 1; //van startup naar idle (NC)
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);  //MCU LED off

            Init_ADC_EG91();
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4); //ADC Enable HIGH EG91 A1

            Systick_delay(500);

            if(EG91_Startup() == 0)//succesful and SIM detected
            {
                Module_status = 2; //van idle (NC) tot idle (C)
                Init_4G();

                Module_status = 3; //idle (C)
                Connect_4G();

                Module_status = 4; //van idle (C) naar transmissie naar idle (C)
                Transmit_4G();

                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4); //ADC Enable LOW EG91 A1
                Module_status = 0;

                for (j = 0; j < 4; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }

                EG_Network_Check();

                Disable_UART(EUSCI_A3_BASE);
            }

            else //failed
            {
                for(j = 0; j<15; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }
            }

            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4); //ADC Enable LOW EG91 A1
            Disable_ADC();
            Reset_Index();

        }
    }

    /*-------------------LORA Button Sequence----------------------*/
    if (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN6) == 0) //LORA button
    {
        while (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN6) == 0)
        {
            Module_status = 1; //van startup naar idle (NC)
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);   //MCU LED low

            Init_ADC_EMB();
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5);  //ADC Enable HIGH EMB-LR1272 A3

            Systick_delay(500);

            if (EMB_Startup() == 0)
            {
                Module_status = 2; //van idle (NC) tot idle (C)
                InitLoRaWAN();

                Module_status = 3; //idle (C)
                ConnectLoRaWAN();

                Module_status = 4; //van idle (C) naar transmissie naar idle (C)
                TransmitLoRaWAN();

                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);  //ADC Enable HIGH EMB-LR1272 A3
                Module_status = 0;

                for (j = 0; j < 4; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }

                Disable_UART(EUSCI_A1_BASE);

                if (EG91_Startup() == 0)
                {
                    EG_Network_Check();
                }

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
            Disable_ADC();
            Reset_Index();
            Disable_UART(EUSCI_A3_BASE);
        }
    }

    /*-------------------CAT-M Button Sequence----------------------*/
    if (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN7) == 0) //CAT_M button
    {
        while (MAP_GPIO_getInputPinValue(GPIO_PORT_P7, GPIO_PIN7) == 0)
        {
            Module_status = 1; //van startup naar idle (NC)
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);  //MCU LED low

            Init_ADC_BG96();

            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable HIGH BG96 A0

            Systick_delay(500);

            if(BG96_Startup() == 0)
            {

                Module_status = 2; //van idle (NC) tot idle (C)
                Init_CAT_M();

                Module_status = 3; //van idle (NC) tot idle (C)
                Connect_CAT_M();

                uart_puts_BG96("AT+CEDRXS=1,5,\"0010\"\r");
                Systick_delay(500);
                ReceiveCheckQuectel();
                /*
                uart_puts_BG96("AT+QSCLK=1\r");
                Systick_delay(500);
                ReceiveCheckQuectel();
                */
                connect_MyCayenne();

                uart_puts_BG96("AT+QCSQ\r");
                Systick_delay(100);
                DataCheck(0);
                //extract rssi,rsrp,sinr,rsrq

                uart_puts_BG96("AT+CBC\r");
                Systick_delay(100);
                DataCheck(1);
                //extract voltage

                uart_puts_BG96("AT+QTEMP\r");
                Systick_delay(100);
                DataCheck(2);
                //extract pmic,xo,pa temp

                Module_status = 4;

                send_MyCayenne(0,pmic,1); //channel,data,constant
                //send_MyCayenne(1,xo,1);
                //send_MyCayenne(2,pa,1);
                //send_MyCayenne(3,vbat,2);
                //send_MyCayenne(4,rssi,3);
                //send_MyCayenne(5,rsrp,3);
                //send_MyCayenne(6,sinr,4);
                //send_MyCayenne(7,rsrq,4);
                //send_MyCayenne(8,bcl,5);

                close_MyCayenne();

                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable Low BG96 A0
                Module_status = 0;


                for (j = 0; j < 4; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }

                Disable_UART(EUSCI_A0_BASE);

                if (EG91_Startup() == 0)
                {
                    EG_Network_Check();
                }

             }

            else//failed
            {
                for (j = 0; j < 15; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }
            }

            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable Low BG96 A0
            Disable_ADC();
            Reset_Index();
            Disable_UART(EUSCI_A3_BASE);
        }
    }


    /*-------------------NB_IOT Button Sequence----------------------*/
    if (MAP_GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN0) == 0) //NB_IOT button
    {
        while (MAP_GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN0) == 0)
        {

            Module_status = 1; //van startup naar idle (NC)
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);  //MCU LED low

            Init_ADC_BG96();
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable HIGH BG96 A0

            Systick_delay(500);

            if(BG96_Startup() == 0)
            {

                Module_status = 2; //van idle (NC) tot idle (C)
                Init_NB_IOT();

                Module_status = 3; //van idle (NC) tot idle (C)
                Connect_NB_IOT();

                Module_status = 4; //van idle (NC) tot idle (C)
                Transmit_NB_IOT();

                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0); //ADC Enable HIGH BG96 A0
                Module_status = 0;

                for (j = 0; j < 4; j++)
                {
                    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P8, GPIO_PIN6); //MCU LED on
                    Systick_delay(100);
                }

                Disable_UART(EUSCI_A0_BASE);

                if (EG91_Startup() == 0)
                {
                    EG_Network_Check();
                }

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
            Disable_ADC();
            Reset_Index();
            Disable_UART(EUSCI_A3_BASE);
        }
    }
}

#endif /* GPIO_H_ */
