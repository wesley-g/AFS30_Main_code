#ifndef MSP_UART_H_
#define MSP_UART_H_

/////////////* function prototypes *//////////////////
void UART_A0_Init(void); //BG96
void UART_A1_Init(void); //EMB
void UART_A2_Init(void); //INP6000
void UART_A3_Init(void); //EG91
void Disable_UART(uint32_t UART_Module);
//////////////////////////////////////////////////////

//UART Baud rate = 115200 baud
const eUSCI_UART_Config EG91_UART_Config =                 // UART calculator: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,                   // SMCLK Clock Source
     6,                                                // BRDIV
     8,                                                // UCxBRF
     32,                                               // UCxBRS
     EUSCI_A_UART_NO_PARITY,                           // No Parity
     EUSCI_A_UART_LSB_FIRST,                           // LSB First
     EUSCI_A_UART_ONE_STOP_BIT,                        // One stop bit
     EUSCI_A_UART_MODE,                                // UART mode
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION     // Oversampling
};

//UART Baud rate = 115200 baud
const eUSCI_UART_Config BG96_UART_Config =                 // UART calculator: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,                   // SMCLK Clock Source
     6,                                                // BRDIV
     8,                                                // UCxBRF
     32,                                               // UCxBRS
     EUSCI_A_UART_NO_PARITY,                           // No Parity
     EUSCI_A_UART_LSB_FIRST,                           // LSB First
     EUSCI_A_UART_ONE_STOP_BIT,                        // One stop bit
     EUSCI_A_UART_MODE,                                // UART mode
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION     // Oversampling
};

//UART Baud rate = 9600 baud
const eUSCI_UART_Config EMB_UART_Config =                 // UART calculator: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,                   // SMCLK Clock Source
     78,                                               // BRDIV
     2,                                                // UCxBRF
     0,                                                // UCxBRS
     EUSCI_A_UART_NO_PARITY,                           // No Parity
     EUSCI_A_UART_LSB_FIRST,                           // LSB First
     EUSCI_A_UART_ONE_STOP_BIT,                        // One stop bit
     EUSCI_A_UART_MODE,                                // UART mode
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION     // Oversampling
};

//UART Baud rate = 38400 baud
const eUSCI_UART_Config INP6000_UART_Config =                 // UART calculator: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,                   // SMCLK Clock Source
     19,                                                // BRDIV
     8,                                                // UCxBRF
     85,                                               // UCxBRS
     EUSCI_A_UART_NO_PARITY,                           // No Parity
     EUSCI_A_UART_LSB_FIRST,                           // LSB First
     EUSCI_A_UART_ONE_STOP_BIT,                        // One stop bit
     EUSCI_A_UART_MODE,                                // UART mode
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION     // Oversampling
};

void UART_A0_Init(void) //BG96 UART interface
{
    /* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); ////////////////////

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A0_BASE, &BG96_UART_Config); /////////////////////////////////

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A0_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
}

void UART_A1_Init(void) //EMB UART interface
{
    /* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A1_BASE, &EMB_UART_Config);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A1_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA1);
}

void UART_A2_Init(void) //INP UART interface
{
    /* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); ////////////////////

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, &EMB_UART_Config); /////////////////////////////////inp CONFIG

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
}

void UART_A3_Init(void) //EG91 UART interface
{
    /* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P9, GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION); ////////////////////

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A3_BASE, &EG91_UART_Config); /////////////////////////////////

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A3_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A3_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA3);
}

void Disable_UART(uint32_t UART_Module)
{
    MAP_UART_disableInterrupt(UART_Module, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_UART_disableModule(UART_Module);
}


#endif /* MSP_UART_H_ */
