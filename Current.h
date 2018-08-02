#ifndef CURRENT_H_
#define CURRENT_H_

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Init_ADC_BG96(void);
void Init_ADC_EG91(void);
void Init_ADC_EMB(void);
void Init_ADC_WiFi(void);
void Init_ADC(void);
void Current_handler(void);

/* Statics */
static volatile uint_fast16_t resultsBuffer[UINT8_MAX];

const int numReadings = 13; // 32768 / 252 = 130
const int numMovAvgs = 250;
const int numMovAvg = 250;

static volatile uint_fast16_t Readings[numReadings];
static volatile uint_fast16_t MovAvgs[numMovAvgs];

static volatile uint_fast16_t MovAvgIdle[numMovAvg];         //van startup naar idle (NC)                  1
static volatile uint_fast16_t MovAvgInitialising[numMovAvg]; //van idle (NC) tot idle (C)                  2
static volatile uint_fast16_t MovAvgConnected[numMovAvg];    //idle (C)                                    3
static volatile uint_fast16_t MovAvgTransmission[numMovAvg]; //vanaf idle (C) tot idle (C) na zenden       4

static volatile int ReadingsIndex = 0;
static volatile int MovAvgsIndex = 0;

static volatile int MovAvgIdleIndex = 0;
static volatile int MovAvgInitialisingIndex = 0;
static volatile int MovAvgConnectedIndex = 0;
static volatile int MovAvgTransmissionIndex = 0;

static volatile uint16_t Readingstotal = 0;
static volatile uint16_t CurrentAverage = 0;
static volatile uint16_t MovAvgtotal = 0;
static volatile uint16_t count = 0;


/* Timer_A Continuous Mode Configuration Parameter */
const Timer_A_UpModeConfig upModeConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,            // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,       // ACLK/1 = 32Khz
        252,                                 // 16384
        TIMER_A_TAIE_INTERRUPT_DISABLE,      // Disable Timer ISR
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disable CCR0
        TIMER_A_DO_CLEAR                     // Clear Counter
};

/* Timer_A Compare Configuration Parameter */
const Timer_A_CompareModeConfig compareConfig =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_SET_RESET,               // Toggle output but
        252                                         // 16000 Period
};

void Init_ADC_BG96(void)
{
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION); //BG96
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false); //BG96

    Init_ADC();
}

void Init_ADC_EG91(void)
{
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION); //EG91
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A1, false); //EG91

    Init_ADC();
}

void Init_ADC_EMB(void)
{
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN2, GPIO_TERTIARY_MODULE_FUNCTION); //EMB
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A3, false); //EMB

    Init_ADC();
}

void Init_ADC_WiFi(void)
{
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN3, GPIO_TERTIARY_MODULE_FUNCTION); //WIFI
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A2, false); //WIFI

    Init_ADC();
}

void Init_ADC(void)
{
    /* Configuring Timer_A in continuous mode and sourced from ACLK */
    MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &upModeConfig);

    /* Configuring Timer_A0 in CCR1 to trigger at 16000 (0.5s) */
    MAP_Timer_A_initCompare(TIMER_A0_BASE, &compareConfig);

    /* Configuring the sample trigger to be sourced from Timer_A0  and setting it
     * to automatic iteration after it is triggered*/
    MAP_ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE1, false);

    /* Enabling the interrupt when a conversion on channel 1 is complete and
     * enabling conversions */
    MAP_ADC14_enableInterrupt (ADC_INT0);
    MAP_ADC14_enableConversion();

    /* Enabling Interrupts */
    MAP_Interrupt_enableInterrupt (INT_ADC14);
    MAP_Interrupt_enableMaster();

    /* Starting the Timer */
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

void Current_handler(void)
{
    if (Module_status == 1)
    {
        MovAvgIdle[MovAvgIdleIndex] = CurrentAverage;
        MovAvgIdleIndex++;

        if(MovAvgIdleIndex >= numMovAvg)
        {
            MovAvgIdleIndex = 0;
        }
    }

    if (Module_status == 2)
    {
        MovAvgInitialising[MovAvgInitialisingIndex] = CurrentAverage;
        MovAvgInitialisingIndex++;

        if(MovAvgInitialisingIndex >= numMovAvg)
        {
            MovAvgInitialisingIndex = 0;
        }
    }

    if (Module_status == 3)
    {
        MovAvgConnected[MovAvgConnectedIndex] = CurrentAverage;
        MovAvgConnectedIndex++;

        if( MovAvgConnectedIndex >= numMovAvg)
        {
            MovAvgConnectedIndex = 0;
        }
    }

    if (Module_status == 4)
    {
        MovAvgTransmission[MovAvgTransmissionIndex] = CurrentAverage;
        MovAvgTransmissionIndex++;

        if(MovAvgTransmissionIndex >= numMovAvg)
        {
            MovAvgTransmissionIndex = 0;
        }
    }
}



#endif /* CURRENT_H_ */
