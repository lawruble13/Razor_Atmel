/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */


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
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{
  for(u8 i = 0; i < 8; i++) LedOff((LedNumberType)i);
  LCDClearChars(0,20);
  LCDClearChars(40,20);
  LCDMessage(0,"Calculator!");
  LCDMessage(40,"^     v      >     =");
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
  }

} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserAppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
    static u8 CurrentDigit=0;
    static u32 CurrentNumber=0;
    static u8 CurrentNumberIndex=0;
    static u32 PastNumbers[10];
    static bool hasPressedNotEquals=FALSE;
    static u32 DisplayNumber=0;
    static bool hasChanged=FALSE;
    static u32 Timer;
    static u8 Controls[21] = "^     v      >     =";
    static u8 Display[21];
    u32 ForMath=0;
    
    if(Timer > 0){
      Timer++;
    }
    //test
    if(Timer == 500){
      LedOn(LCD_BLUE);
      LedOn(LCD_GREEN);
      LedOn(LCD_RED);
    }
    
    if(WasButtonPressed(BUTTON0)){
      ButtonAcknowledge(BUTTON0);
      hasPressedNotEquals=TRUE;
      CurrentDigit++;
      if(CurrentDigit > 9){
        CurrentDigit = 0;
      }
      DisplayNumber = 10*CurrentNumber+CurrentDigit;
      hasChanged=TRUE;
    }
    if(WasButtonPressed(BUTTON1)){
      ButtonAcknowledge(BUTTON1);
      hasPressedNotEquals=TRUE;
      if(CurrentDigit >= 1){
        CurrentDigit--;
      } else {
        CurrentDigit = 9;
      }
      DisplayNumber = 10*CurrentNumber+CurrentDigit;
      hasChanged=TRUE;
    }
    if(WasButtonPressed(BUTTON2)){
      ButtonAcknowledge(BUTTON2);
      hasPressedNotEquals=TRUE;
      if(CurrentNumber < 42949672){
        CurrentNumber *= 10;
        CurrentNumber += CurrentDigit;
        CurrentDigit=0;
        DisplayNumber=10*CurrentNumber;
        hasChanged=TRUE;
      } else {
        LedOff(LCD_GREEN);
        LedOff(LCD_BLUE);
        LedOn(LCD_RED);
        Timer=1;
      }
    }
    if(WasButtonPressed(BUTTON3)){
      ButtonAcknowledge(BUTTON3);
      if(!hasPressedNotEquals){
        for(int i=0;i<10;i++){
          ForMath += PastNumbers[i];
          PastNumbers[i]=0;
        }
        DisplayNumber=ForMath;
        ForMath=0;
      } else {
        CurrentNumber *= 10;
        CurrentNumber += CurrentDigit;
        PastNumbers[CurrentNumberIndex]=CurrentNumber;
        CurrentNumber=0;
        CurrentDigit=0;
        CurrentNumberIndex++;
        if(CurrentNumberIndex > 9){
          CurrentNumberIndex=0;
          LedOff(LCD_GREEN);
          LedOff(LCD_BLUE);
          LedOn(LCD_RED);
        }
        DisplayNumber=0;
        hasPressedNotEquals=FALSE;
      }
      hasChanged=TRUE;
    }
    if(hasChanged){
      LCDClearChars(0,20);
      u32 x=0;
      u8 c=0;
      while(c < 20){
        x=DisplayNumber%10;
        DisplayNumber/=10;
        if(DisplayNumber || x){
          Display[19-c]=48+x;
        } else {
          Display[19-c]=' ';
        }
        c++;
      }
      Display[20]='\0';
      LCDMessage(0,Display);
      LCDClearChars(40,20);
      if(hasPressedNotEquals){
        Controls[19]='+';
      } else {
        Controls[19]='=';
      }
      LCDMessage(40,Controls);
      hasChanged = FALSE;
    }
} /* end UserAppSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
