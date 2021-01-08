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
#include "SubHSM_Destroy.h"
#include "Robot.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    Check,
    Back,
    Forward,
    Lineup,
    Fire,
    Stop,
} TemplateSubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"Check",
	"Back",
	"Forward",
	"Lineup",
	"Fire",
	"Stop",
};

#define SHOOT_TIMER 5000 //Time needed to deposit one ball
#define BACK_TIMER 500 //Timer to become parallel with the beacon and see if there is a tape
#define FORWARD_TIMER 1000 //Timer to move forward

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
uint8_t InitSubHSM_Destroy(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunSubHSM_Destroy(INIT_EVENT);
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
ES_Event RunSubHSM_Destroy(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    TemplateSubHSMState_t nextState; // <- change type to correct enum
    ES_Event ReturnEvent;
    static ES_EventTyp_t lastEvent = No_Ball_deposit;
    ES_EventTyp_t curEvent = No_Ball_deposit;
    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state
                // now put the machine into the actual initial state
                nextState = Back;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        case Back: // in the first state, Move right motors backwards to line up the tape sensor with the beacon tower
            switch (ThisEvent.EventType) {
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            if (ThisEvent.EventType == ES_TIMEOUT) {
                if (ThisEvent.EventParam == DESTROY_TIMER) {//if tape is not switching to no tape, then we know it is already on the tape
                    nextState = Forward;
                    ES_Timer_InitTimer(DESTROY_TIMER, FORWARD_TIMER);
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (ThisEvent.EventParam == HSM_TIMER) {
                    nextState = Forward;
                    ES_Timer_InitTimer(DESTROY_TIMER, FORWARD_TIMER);
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                }
            } else if (ThisEvent.EventType == CannonTape) { //tape sensor should read no tape once it is close to the beacon tower, then we know we are parallel to it
                nextState = Lineup;
                makeTransition = TRUE;
            }
            Robot_RightMtrSpeed(-80);
            Robot_LeftMtrSpeed(-80);
            break;
        case Forward: //move forward for tape for about a second
            if (ThisEvent.EventType == ES_TIMEOUT) {
                nextState = Back;
                ES_Timer_InitTimer(DESTROY_TIMER, BACK_TIMER);
                makeTransition = TRUE;
            } else if (ThisEvent.EventType == CannonTape) {
                nextState = Lineup;
                makeTransition = TRUE;
            }
            Robot_RightMtrSpeed(100);
            Robot_LeftMtrSpeed(90);
            break;
        case Lineup: //if the tape was not forward then we move back to find the tape
            if (ThisEvent.EventType == NoCannonTape) {
                nextState = Fire;
                ES_Timer_InitTimer(DESTROY_TIMER, SHOOT_TIMER);
                makeTransition = TRUE;
            } else {
                Robot_RightMtrSpeed(-70);
                Robot_LeftMtrSpeed(-70);
            }
            break;
        case Fire: //Move cannon motor for about 1.2 seconds
            if (ThisEvent.EventType == ES_TIMEOUT) {
                if (ThisEvent.EventParam == DESTROY_TIMER) {
//                    printf("Stop \n");
                    CannonMtrSpeed(0);
                    curEvent = Ball_deposit;
                    ES_Timer_InitTimer(DESTROY_TIMER, BACK_TIMER);
                    ThisEvent.EventType = ES_NO_EVENT;
                }
            }
//            printf("Shoot \n");
            Robot_RightMtrSpeed(0);
            Robot_LeftMtrSpeed(0);
            CannonMtrSpeed(75);
            break;
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunSubHSM_Destroy(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunSubHSM_Destroy(ENTRY_EVENT); // <- rename to your own Run function
    }
    if (curEvent != lastEvent) {
        ReturnEvent.EventType = curEvent;
        ReturnEvent.EventParam = (uint16_t) curEvent;
        lastEvent = curEvent;
        PostRobotHSM(ReturnEvent);
    }
    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

