/*
 * File:   BumperService.c
 * Author: mlira2
 *
 * Created on October 4, 2019, 12:33 AM
 */



/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "BOARD.h"
#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "RobotBumper.h" 
#include <stdio.h>
#include <Robot.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

//#define BATTERY_DISCONNECT_THRESHOLD 175
#define TIMER_1_TICKS 5

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/
/* You will need MyPriority and maybe a state variable; you may need others
 * as well. */

static uint8_t MyPriority;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateService function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitRobotBumper(uint8_t Priority) {

    ES_Event ThisEvent;

    MyPriority = Priority;

    // in here you write your initialization code
    // this includes all hardware and software initialization
    // that needs to occur.
    //    BOARD_Init();
    //    Robot_Init(); 

    // post the initial transition event
    ES_Timer_InitTimer(BUMPER_SERVICE_TIMER, TIMER_1_TICKS);
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostRobotBumper(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 *        as this is called any time a new event is passed to the event queue. 
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunRobotBumper(ES_Event ThisEvent) {
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    /********************************************
     in here you write your service code
     *******************************************/
    static ES_EventTyp_t lastEvent1 = NoFrontRightBump;
    static ES_EventTyp_t lastEvent2 = NoFrontLeftBump;
    static ES_EventTyp_t lastEvent3 = NoSideBump;

    ES_EventTyp_t curEvent1 = NoFrontRightBump;
    ES_EventTyp_t curEvent2 = NoFrontLeftBump;
    ES_EventTyp_t curEvent3 = NoSideBump;

    switch (ThisEvent.EventType) {
        case ES_INIT:
            // No hardware initialization or single time setups, those
            // go in the init function above.
            //
            // This section is used to reset service for some reason
            break;

        case ES_TIMERACTIVE:

        case ES_TIMERSTOPPED:
            break;

        case ES_TIMEOUT:
            if (Robot_ReadBumpers() & 0x02) {
                if (Robot_ReadBumpers() & 0x02) {
                    if (Robot_ReadBumpers() & 0x02) { // now we can count 
                        curEvent1 = FrontRightBump;
                    }
                }
            } else if (Robot_ReadBumpers() & 0x01) {
                if (Robot_ReadBumpers() & 0x01) {
                    if (Robot_ReadBumpers() & 0x01) {
                        curEvent2 = FrontLeftBump;
                    }
                }
            } else if (Robot_ReadBumpers() & 0x04) {
                if (Robot_ReadBumpers() & 0x04) {
                    if (Robot_ReadBumpers() & 0x04) {
                        if (Robot_ReadBumpers() & 0x04) {
                            if (Robot_ReadBumpers() & 0x04) {
                                curEvent3 = SideBump;
                            }
                        }
                    }
                }
            }
            if (curEvent1 != lastEvent1) { // check for change from last time
                ReturnEvent.EventType = curEvent1;
                ReturnEvent.EventParam = (uint16_t) curEvent1;
                lastEvent1 = curEvent1;
                PostRobotHSM(ReturnEvent);
            }// update history
            if (curEvent2 != lastEvent2) { // check for change from last time
                ReturnEvent.EventType = curEvent2;
                ReturnEvent.EventParam = (uint16_t) curEvent2;
                lastEvent2 = curEvent2; // update history
                PostRobotHSM(ReturnEvent);
            }
            if (curEvent3 != lastEvent3) { // check for change from last time
                ReturnEvent.EventType = curEvent3;
                ReturnEvent.EventParam = (uint16_t) curEvent3;
                lastEvent3 = curEvent3; // update history
                PostRobotHSM(ReturnEvent);
            }
            ES_Timer_InitTimer(BUMPER_SERVICE_TIMER, TIMER_1_TICKS);

            /*#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                        PostRobotBumper(ReturnEvent);
            #else
                        PostRobotHSM(ReturnEvent);
            #endif   */
            break;
#ifdef SIMPLESERVICE_TEST     // keep this as is for test harness      
        default:
            printf("\r\nEvent: %s\tParam: 0x%X",
                    EventNames[ThisEvent.EventType], ThisEvent.EventParam);
            break;
#endif
    }

    return ReturnEvent;
}

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/

