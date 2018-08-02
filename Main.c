/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
//#include <string.h>


#include <EMB_LR1272.h>      // EMBIT header file
#include <BG96.h>            // Quectel BG96 header file
#include <EG91.h>            // Quectel EG91 header file
#include <INP6000.h>         // Innophase header file
#include <MSP_UART.h>        // UART_header file
#include <Message_handler.h> // message handling header file
#include <GPIO.h>            // GPIO header file
#include <Current.h>         // current monitoring header file

void Systick_delay(uint16_t time);

volatile uint32_t Ticks = 0;

/* Vairables to cloud */
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
int Samples;       //Current sample data
char StringToSend[150];


int main(void)
{
    /* Halting WDT  */
    MAP_WDT_A_holdTimer();
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    Interrupt_setPriority(INT_EUSCIA0 , 0x00); // highest priority
    Interrupt_setPriority(INT_EUSCIA1 , 0x00); // highest priority
    Interrupt_setPriority(INT_EUSCIA2 , 0x00); // highest priority
    Interrupt_setPriority(INT_EUSCIA3 , 0x00); // highest priority
    Interrupt_setPriority(INT_ADC14 , 0x20);   // lower priority

    /* Configuring Systick */
    MAP_SysTick_enableModule();
    MAP_SysTick_setPeriod(12000); //1 ms
    MAP_SysTick_enableInterrupt();

    /* Setting up clocks
     * MCLK = MCLK = 3MHz
     * ACLK = REFO = 32Khz */
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initializing ADC (MCLK/1/1) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);

    MCU_led();
    Init_Buttons();

    Network         = 1;    //active network
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

    char str1[] = Network + '0';
    char str2[] = GNSS_BG96 + '0';
    char str3[] = SignalStrength + '0';
    char str4[] = GPS_Lat + '0';
    char str5[] = GPS_Lon + '0';
    char str6[] = MCC + '0';
    char str7[] = MNC + '0';
    char str8[] = LAC + '0';
    char str9[] = Cell + '0';
    char str10[] = Size + '0';
    char str11[] = Sequence + '0';
    char str12[] = Samples + '0';


    sprintf(StringToSend, "{\"network\":%s,\"bg96\":%s,\"signal\":%s,\"lat\":%f,\"lon\":%f,\"mcc\":%s,\"mnc\":%s,\"lac\":%s,\"cid\":%s,\"size\":%s,\"sequence\":%s,\"samples\":%s}",str1 ,str2 ,str3 ,str4 ,str5 ,str6 ,str7, str8, str9, str10, str11, str12);

    puts(StringToSend);


    /*  StringToSend = {
        "{network:%d,",Network,
        "bg96:%d,",GNSS_BG96,
        "signal:%d,",SignalStrength,
        "lat:%d,",GPS_Lat,
        "lon:%d,",GPS_Lon,
        "mcc:%d,",MCC,
        "mnc:0%d,",MNC,
        "lac:%d,",LAC,
        "cid:%d,",Cell,
        "size:%d,",Size,
        "sequence:%d,",Sequence,
        "samples:%d}\r",Samples}; */

    //,bg96:%d,signal:%d,lat:%d,lon:%d,mcc:%d,mnc:%d,lac:%d,cid:%d,size:%d,sequence:%d,samples:%d,  ,GNSS_BG96,SignalStrength,GPS_Lat,GPS_Lon,MCC,MNC,LAC,Cell,Size,Sequence,Samples

    while (1)
    {
        Check_Buttons();
    }

} // end of main

/* EUSCI A0 UART ISR */
void EUSCIA0_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        RXBuffer[i] = (char *) MAP_UART_receiveData(EUSCI_A0_BASE);
        i++;
    }
}

/* EUSCI A1 UART ISR */
void EUSCIA1_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A1_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A1_BASE, status);

    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        RXBuffer[i] = (char *) MAP_UART_receiveData(EUSCI_A1_BASE);
        i++;
    }
}


/* EUSCI A2 UART ISR */
void EUSCIA2_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        RXBuffer[i] = (char *) MAP_UART_receiveData(EUSCI_A2_BASE);
        i++;
    }
}

/* EUSCI A3 UART ISR */
void EUSCIA3_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A3_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A3_BASE, status);

    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        RXBuffer[i] = (char *) MAP_UART_receiveData(EUSCI_A3_BASE);
        i++;
    }
}

/* ADC14 ISR */
void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    if (status & ADC_INT0)
    {
        Readingstotal = Readingstotal - Readings[ReadingsIndex];
        Readings[ReadingsIndex] = MAP_ADC14_getResult(ADC_MEM0);

        Readingstotal = Readingstotal + Readings[ReadingsIndex++];

        if (ReadingsIndex >= numReadings)
        {
            ReadingsIndex = 0;

            MovAvgtotal = MovAvgtotal - MovAvgs[MovAvgsIndex];
            MovAvgs[MovAvgsIndex] = CurrentAverage;
            Current_handler();
            MovAvgtotal = MovAvgtotal + MovAvgs[MovAvgsIndex++];

            if (MovAvgsIndex >= numMovAvgs)           // if end of array restart
            {
                MovAvgsIndex = 0;
            }
        }

        CurrentAverage = Readingstotal / numReadings; // 1 average has been calculated
    }
}

void Systick_delay(uint16_t time)
{
    uint16_t start =Ticks;
    while(Ticks-start < time);
}

void SysTick_Handler(void)
{
    Ticks++;
}
