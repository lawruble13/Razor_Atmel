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
    static bigUInt CurrentNumber = {NULL,0};
    static u8 CurrentNumberIndex=0;
    static bigUInt PastNumbers[32];
    static bigUInt min20Digs = {NULL, 0};
    static u8 defaultOperator = '=';
    static bigUInt DisplayNumber={{0},1};
    static bool hasChanged=FALSE;
    static u32 Timer;
    static u8 Controls[21] = "^     v      >     =";
    static u8 Display[21];
    static u32 HeldCount;
    
    if(min20Digs.length == 0){
      min20Digs.digits = malloc(20);
      for(int i = 0; i < 19; i++){
        min20Digs.digits[i] = 0;
      }
      min20Digs.digits[19] = 1;
      min20Digs.length = 20;
    }
    
    if(Timer > 0){
      Timer++;
    }
    if(Timer == 500){
      LedOn(LCD_BLUE);
      LedOn(LCD_GREEN);
      LedOn(LCD_RED);
    }
    
    if(WasButtonPressed(BUTTON0)){
      ButtonAcknowledge(BUTTON0);
      HeldCount=0;
      defaultOperator = '+';
      CurrentDigit++;
      if(CurrentDigit > 9){
        CurrentDigit = 0;
      }
      bigUInt_copy(&DisplayNumber, CurrentNumber);
      DisplayNumber.digits = realloc(DisplayNumber.digits, DisplayNumber.length+1);
      for(int i = DisplayNumber.length; i > 0; i--){
        DisplayNumber.digits[i] = DisplayNumber.digits[i-1];
      }
      DisplayNumber.digits[0] = CurrentDigit;
      DisplayNumber.length++;
      hasChanged=TRUE;
    }
    if(IsButtonHeld(BUTTON0, (u32) 200*(HeldCount+3))){
      CurrentDigit++;
      if(CurrentDigit > 9){
        CurrentDigit=0;
      }
      bigUInt_copy(&DisplayNumber, CurrentNumber);
      DisplayNumber.digits = realloc(DisplayNumber.digits, DisplayNumber.length+1);
      for(int i = DisplayNumber.length; i > 0; i--){
        DisplayNumber.digits[i] = DisplayNumber.digits[i-1];
      }
      DisplayNumber.digits[0] = CurrentDigit;
      DisplayNumber.length++;
      hasChanged=TRUE;
      HeldCount++;
    }
    
    if(WasButtonPressed(BUTTON1)){
      HeldCount=0;
      ButtonAcknowledge(BUTTON1);
      defaultOperator = '+';
      if(CurrentDigit >= 1){
        CurrentDigit--;
      } else {
        CurrentDigit = 9;
      }
      bigUInt_copy(&DisplayNumber, CurrentNumber);
      DisplayNumber.digits = realloc(DisplayNumber.digits, DisplayNumber.length+1);
      for(int i = DisplayNumber.length; i > 0; i--){
        DisplayNumber.digits[i] = DisplayNumber.digits[i-1];
      }
      DisplayNumber.digits[0] = CurrentDigit;
      DisplayNumber.length++;
      hasChanged=TRUE;
    }
    if(IsButtonHeld(BUTTON1, (u32) 200*(HeldCount+3))){
      if(CurrentDigit >= 1){
        CurrentDigit--;
      } else {
        CurrentDigit = 9;
      }
      bigUInt_copy(&DisplayNumber, CurrentNumber);
      DisplayNumber.digits = realloc(DisplayNumber.digits, DisplayNumber.length+1);
      for(int i = DisplayNumber.length; i > 0; i--){
        DisplayNumber.digits[i] = DisplayNumber.digits[i-1];
      }
      DisplayNumber.digits[0] = CurrentDigit;
      DisplayNumber.length++;
      hasChanged=TRUE;
      HeldCount++;
    }
    
    if(WasButtonPressed(BUTTON2)){
      ButtonAcknowledge(BUTTON2);
      defaultOperator='+';
      if(bigUInt_lessthan(CurrentNumber,min20Digs)){
        CurrentNumber.digits = realloc(CurrentNumber.digits, CurrentNumber.length+1);
        for(int i = CurrentNumber.length; i > 0; i--){
          CurrentNumber.digits[i] = CurrentNumber.digits[i-1];
        }
        CurrentNumber.digits[0] = CurrentDigit;
        CurrentNumber.length++;
        CurrentDigit=0;
        bigUInt_copy(&DisplayNumber, CurrentNumber);
        DisplayNumber.digits = realloc(DisplayNumber.digits, DisplayNumber.length+1);
        for(int i = DisplayNumber.length; i > 0; i--){
          DisplayNumber.digits[i] = DisplayNumber.digits[i-1];
        }
        DisplayNumber.digits[0] = 0;
        DisplayNumber.length++;
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
      if(defaultOperator == '='){
        bigUInt ForMath;
        ForMath = bigUInt_from_u32(0);
        for(int i=0;i<10;i++){
          bigUInt_add_2_p(&ForMath,PastNumbers[i]);
          free(PastNumbers[i].digits);
          PastNumbers[i].digits = NULL;
          PastNumbers[i].length = 0;
        }
        bigUInt_copy(&DisplayNumber,ForMath);
        free(ForMath.digits);
        CurrentNumberIndex=0;
        LedOn(LCD_RED);
        LedOn(LCD_GREEN);
        LedOn(LCD_BLUE);
      } else {
        CurrentNumber.digits = realloc(CurrentNumber.digits, CurrentNumber.length+1);
        for(int i = CurrentNumber.length; i > 0; i--){
          CurrentNumber.digits[i] = CurrentNumber.digits[i-1];
        }
        CurrentNumber.digits[0] = CurrentDigit;
        CurrentNumber.length++;
        bigUInt_copy(&PastNumbers[CurrentNumberIndex],CurrentNumber);
        free(CurrentNumber.digits);
        CurrentNumber.digits = NULL;
        CurrentNumber.length = 0;
        CurrentDigit=0;
        CurrentNumberIndex++;
        if(CurrentNumberIndex > 31){
          CurrentNumberIndex=0;
          LedOff(LCD_GREEN);
          LedOff(LCD_BLUE);
          LedOn(LCD_RED);
        }
        free(DisplayNumber.digits);
        DisplayNumber.digits = NULL;
        DisplayNumber.length = 0;
        defaultOperator='=';
      }
      hasChanged=TRUE;
    }
    if(hasChanged){
      LCDClearChars(LINE1_START_ADDR,20);
      u32 x=0;
      u8 c=0;
      while(c < 20){
        if(c < DisplayNumber.length){
          x = 48 + DisplayNumber.digits[c];
        } else {
          x = ' ';
        }
        Display[19-c]=x;
        c++;
      }
      Display[20]='\0';
      LCDMessage(0,Display);
      LCDClearChars(LINE2_START_ADDR,20);
      Controls[19] = defaultOperator;
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
