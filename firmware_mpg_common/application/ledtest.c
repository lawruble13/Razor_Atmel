/**********************************************************************************************************************
File: ledtest.c                                                                

----------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------

Description:
This is a ledtest.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void LEDTestInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void LEDTestRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"
#include "leds.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32LEDTestFlags; 

/* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "LEDTest_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type LEDTest_StateMachine;            /* The state machine function pointer */
static u32 LEDTest_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: LEDTestInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void LEDTestInitialize(void)
{
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
  LedPWM(LCD_RED,LED_PWM_100);
  LedPWM(LCD_GREEN,LED_PWM_0);
  LedPWM(LCD_BLUE,LED_PWM_0);
  LCDClearChars((u8)0,(u8)40);
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    LEDTest_StateMachine = LEDTestSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    LEDTest_StateMachine = LEDTestSM_FailedInit;
  }

} /* end LEDTestInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function LEDTestRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void LEDTestRunActiveState(void)
{
  LEDTest_StateMachine();

} /* end LEDTestRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void LEDTestSM_Idle(void)
{
  static u16 u16BinaryCounter  = 0;
  static u8 u8BinaryCounter    = 0;
  static LedNumberType aeCurrentLed[] = {LCD_GREEN,LCD_RED,LCD_BLUE,LCD_GREEN,LCD_RED,LCD_BLUE};
  static bool abLedRateIncreasing[]   = {TRUE,     FALSE,  TRUE,    FALSE,    TRUE,   FALSE};
  static u8 u8CurrentLedIndex  = 0;
  static u8 u8LedCurrentLevel  = 0;
  static u8 u8DutyCycleCounter = 0;
  static u16 u16Counter = COLOR_CYCLE_TIME;
  static bool CyclingOn = TRUE;
  u16BinaryCounter++;
  if(CyclingOn)
    u16Counter--;
  if(u16Counter == 0){
    u16Counter = COLOR_CYCLE_TIME;
    LedPWM((LedNumberType)aeCurrentLed[u8CurrentLedIndex], (LedRateType)u8LedCurrentLevel);
    if(abLedRateIncreasing[u8CurrentLedIndex]){
      u8LedCurrentLevel++;
    } else {
      u8LedCurrentLevel--;
    }
    u8DutyCycleCounter++;
    if(u8DutyCycleCounter == 20){
      u8DutyCycleCounter = 0;
      
      u8CurrentLedIndex++;
      if(u8CurrentLedIndex == sizeof(aeCurrentLed))
        u8CurrentLedIndex = 0;
      
      u8LedCurrentLevel=20;
      if(abLedRateIncreasing[u8CurrentLedIndex])
        u8LedCurrentLevel=0;
    }
  }
  if(WasButtonPressed(BUTTON0)){
    ButtonAcknowledge(BUTTON0);
    CyclingOn = (bool)!CyclingOn;
  }
  if(u16BinaryCounter == 500){
    u16BinaryCounter=0;
    u8BinaryCounter++;
    if(u8BinaryCounter == 16) u8BinaryCounter = 0;
    (u8BinaryCounter & 0x01) ? LedOn(RED) : LedOff(RED);
    (u8BinaryCounter & 0x02) ? LedOn(ORANGE) : LedOff(ORANGE);
    (u8BinaryCounter & 0x04) ? LedOn(YELLOW) : LedOff(YELLOW);
    (u8BinaryCounter & 0x08) ? LedOn(GREEN) : LedOff(GREEN);
  }
  
} /* end LEDTestSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void LEDTestSM_Error(void)          
{
  
} /* end LEDTestSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void LEDTestSM_FailedInit(void)          
{
    
} /* end LEDTestSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
