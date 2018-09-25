/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <GPIO.h>            // GPIO header file
#include <Current.h>         // current monitoring header file
#include <Message_handler.h> // message handling header file
#include <MSP_UART.h>        // UART_header file
#include <Current.h>         // current monitoring header file

#include <EMB_LR1272.h>      // EMBIT header file
#include <BG96.h>            // Quectel BG96 header file
#include <EG91.h>            // Quectel EG91 header file
#include <INP6000.h>         // Innophase header file


void Systick_delay(uint16_t time);

volatile uint32_t Ticks = 0;

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

    MCU_led();
    Init_Buttons();

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);   //MCU LED on PORT 8 PIN 6

    BG96_Startup();
    BG96_setting();
    EG91_Startup();

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
    /*
    uint16_t start = Ticks;
    while(Ticks-start < time);
    */
    Ticks = 0;
    while(Ticks != time);


}

void SysTick_Handler(void)
{
    Ticks++;

    if(Ticks == UINT32_MAX)
    {
        Ticks = 0;
    }

}
