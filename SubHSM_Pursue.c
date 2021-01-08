/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "RobotHSM.h"
#include "SubHSM_Pursue.h"
#include "Robot.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    Pursue,
    adjust,
    Slide,
    Slide2,
    PivotLeft,
    backup,
    Backup2,
    Side,
    bump,
    Check,
    RightTape,
    RightTape2,
    LeftTape,
    TapeBackR,
    TapeBackR2,
    TapeBackL,
    Stop,
    SideFollowOn,
    SideFollowOff,
    HardLeft,
} TemplateSubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"Pursue",
	"adjust",
	"Slide",
	"Slide2",
	"PivotLeft",
	"backup",
	"Backup2",
	"Side",
	"bump",
	"Check",
	"RightTape",
	"RightTape2",
	"LeftTape",
	"TapeBackR",
	"TapeBackR2",
	"TapeBackL",
	"Stop",
	"SideFollowOn",
	"SideFollowOff",
	"HardLeft",
};

#define CHECK_TIMER 1000
#define TANK_TIMER 500
#define TANK_TIMER2 500
#define BACK_UP 150
#define BACK2 150
#define TAPE_BACK 350
#define SIDE1_TIMER 10000
#define SIDE_TIMER 250
#define LEFT_TIMER 3000

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static TemplateSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitSubHSM_Pursue(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunSubHSM_Pursue(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunTemplateSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunSubHSM_Pursue(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    TemplateSubHSMState_t nextState; // <- change type to correct enum
    ES_Event ReturnEvent;
    static ES_EventTyp_t lastEvent = NoSeeking;

    ES_EventTyp_t curEvent = NoSeeking;

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                // now put the machine into the actual initial state
                nextState = Pursue;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        case Pursue: // in the first state, Simply move forward
            switch (ThisEvent.EventType) {
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            if (ThisEvent.EventType == No_Beacon_found) {
                nextState = adjust;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FrontLeftBump) {
                nextState = backup;
                ES_Timer_InitTimer(PURSUE_TIMER, BACK_UP);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FrontRightBump) {
                nextState = backup;
                ES_Timer_InitTimer(PURSUE_TIMER, BACK_UP);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FrontRightTape) {
                nextState = TapeBackR;
                ES_Timer_InitTimer(PURSUE_TIMER, TAPE_BACK);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FrontLeftTape) {
                nextState = TapeBackL;
                ES_Timer_InitTimer(PURSUE_TIMER, TAPE_BACK);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(100);
                Robot_LeftMtrSpeed(85);
            }
            break;
        case backup: //Backup a little so robot can make better right turns
            //            if (ThisEvent.EventType == NoFrontLeftBump) {
            //                nextState = SideFollowOff;
            //                makeTransition = TRUE;
            //                ThisEvent.EventType == ES_NO_EVENT;
            //            } else if (ThisEvent.EventType == SideBump) {
            //                nextState = SideFollowOn;
            //                makeTransition = TRUE;
            //                ThisEvent.EventType = ES_NO_EVENT;
            //            }
            //            Robot_RightMtrSpeed(-100);
            //            Robot_LeftMtrSpeed(100);
            //            break;
            if (ThisEvent.EventType == ES_TIMEOUT) {
                if (ThisEvent.EventParam == PURSUE_TIMER) {
                    nextState = bump;
                    //                ES_Timer_InitTimer(PURSUE2_TIMER, LEFT_TIMER);
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                }
            } else {
                Robot_RightMtrSpeed(-100);
                Robot_LeftMtrSpeed(-100);
            }
            break;
        case bump: //keep hitting beacon until the side bumper hits it
            if (ThisEvent.EventType == FrontRightBump) {
                nextState = backup;
                //                ES_Timer_StopTimer(PURSUE2_TIMER);
                ES_Timer_InitTimer(PURSUE_TIMER, BACK_UP);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else if (ThisEvent.EventType == FrontLeftBump) {
                nextState = backup;
                //                ES_Timer_StopTimer(PURSUE2_TIMER);
                ES_Timer_InitTimer(PURSUE_TIMER, BACK_UP);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else if (ThisEvent.EventType == SideBump) {
                nextState = SideFollowOn;
                ES_Timer_InitTimer(PURSUE_TIMER, BACK_UP);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            //            else if (ThisEvent.EventType == ES_TIMEOUT) {
            //                if (ThisEvent.EventParam == PURSUE2_TIMER);
            //                {
            //                    nextState = HardLeft;
            //                    makeTransition = TRUE;
            //                    ThisEvent.EventType = ES_NO_EVENT;
            //                }
            //            }
            Robot_LeftMtrSpeed(90);
            Robot_RightMtrSpeed(75);
            break;
        case Backup2: //backup a little bit so robot can better turns
            if (ThisEvent.EventType == ES_TIMEOUT) {
                nextState = Side;
                ES_Timer_InitTimer(PURSUE_TIMER, SIDE_TIMER);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(-85);
                Robot_LeftMtrSpeed(0);
            }
            break;
        case Side: //Keep hitting the side bumper until it doesn't hit it within a time limit
            if (ThisEvent.EventType == ES_TIMEOUT) {
                nextState = Check;
                ES_Timer_InitTimer(PURSUE_TIMER, CHECK_TIMER);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == SideBump) {
                nextState = Backup2;
                ES_Timer_InitTimer(PURSUE_TIMER, BACK2);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(75);
                Robot_LeftMtrSpeed(100);
            }
            break;
        case Check: // Check for Track Wire with no noise from motors
            if (ThisEvent.EventType == ES_TIMEOUT) {
                nextState = Slide;
                //            ES_Timer_InitTimer(PURSUE_TIMER, SIDE1_TIMER);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(0);
                Robot_LeftMtrSpeed(0);
            }

            break;
        case Slide: // Move along side of beacon tower by hugging the beacon
            if (ThisEvent.EventType == SideBump) {
                nextState = SideFollowOn;
                ES_Timer_InitTimer(PURSUE_TIMER, SIDE1_TIMER);
                makeTransition = TRUE;
            } else if (ThisEvent.EventType == FrontRightTape) {
                nextState = TapeBackR2;
                ES_Timer_InitTimer(PURSUE_TIMER, TAPE_BACK);
                makeTransition = TRUE;
            }
            Robot_RightMtrSpeed(100);
            Robot_LeftMtrSpeed(60);
            break;
        case SideFollowOn:
            if (ThisEvent.EventType == NoSideBump) {
                nextState = SideFollowOff;
                ES_Timer_InitTimer(PURSUE_TIMER, SIDE1_TIMER);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else if (ThisEvent.EventType == NoFrontLeftBump) {
                nextState = SideFollowOff;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            Robot_LeftMtrSpeed(100);
            Robot_RightMtrSpeed(80);
            break;
        case SideFollowOff:
            if (ThisEvent.EventType == SideBump) {
                nextState = SideFollowOn;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else if (ThisEvent.EventType == ES_TIMEOUT) {
                if (ThisEvent.EventParam == PURSUE_TIMER) {
                    nextState = SideFollowOn;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                }
            } else if (ThisEvent.EventType == FrontLeftBump) {
                nextState = SideFollowOn;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            Robot_LeftMtrSpeed(40);
            Robot_RightMtrSpeed(100);
            break;
        case HardLeft:
            if (ThisEvent.EventType == SideBump) {
                nextState = SideFollowOn;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(100);
                Robot_LeftMtrSpeed(0);
            }
            break;
        case TapeBackR:
            if (ThisEvent.EventType == ES_TIMEOUT) {
                nextState = RightTape;
                ES_Timer_InitTimer(PURSUE_TIMER, TANK_TIMER);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(-100);
                Robot_LeftMtrSpeed(-100);
            }
            break;
        case TapeBackR2: //Reverse to move skid off tape
            if (ThisEvent.EventType == ES_TIMEOUT) {
                nextState = RightTape2;
                ES_Timer_InitTimer(PURSUE_TIMER, TANK_TIMER2);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(-100);
                Robot_LeftMtrSpeed(-100);
            }
            break;
        case RightTape: //Move Left, away from tape to go to seeking again
            if (ThisEvent.EventType == ES_TIMEOUT) {
                curEvent = GoSeeking;
                nextState = Pursue;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(0);
                Robot_LeftMtrSpeed(-100);
            }
            break;
        case RightTape2: //Turn right to begin hugging the inside tape
            if (ThisEvent.EventType == ES_TIMEOUT) {
                curEvent = GoSeeking;
                nextState = Pursue;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(-100);
                Robot_LeftMtrSpeed(0);
            }
            break;
        case TapeBackL:
            if (ThisEvent.EventType == ES_TIMEOUT) {
                nextState = LeftTape;
                ES_Timer_InitTimer(PURSUE_TIMER, TANK_TIMER);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(-100);
                Robot_LeftMtrSpeed(-100);
            }
            break;
        case LeftTape:
            if (ThisEvent.EventType == ES_TIMEOUT) {
                curEvent = GoSeeking;
                nextState = Pursue;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(-100);
                Robot_LeftMtrSpeed(0);
            }
            break;
        case adjust:
            if (ThisEvent.EventType == Beacon_found) {
                nextState = Pursue;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FrontLeftBump) {
                nextState = backup;
                ES_Timer_InitTimer(PURSUE_TIMER, BACK_UP);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FrontRightBump) {
                nextState = backup;
                ES_Timer_InitTimer(PURSUE_TIMER, BACK_UP);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else {
                Robot_RightMtrSpeed(0);
                Robot_LeftMtrSpeed(100);
            }
            break;
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunSubHSM_Pursue(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunSubHSM_Pursue(ENTRY_EVENT); // <- rename to your own Run function
    }
    if (curEvent != lastEvent) {
        ReturnEvent.EventType = curEvent;
        curEvent = NoSeeking;
        PostRobotHSM(ReturnEvent);
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

