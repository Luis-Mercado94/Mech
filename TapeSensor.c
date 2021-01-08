/*
 * File: TapeSensor.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a simple service to work with the Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 13/Nov/2013
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "BOARD.h"
#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "TapeSensor.h"
#include "pwm.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

//#define BATTERY_DISCONNECT_THRESHOLD 175
#define TIMER_1_TICKS 45

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
uint8_t InitTapeSensor(uint8_t Priority)
{
//    PWM_Init();
//    AD_Init();
    ES_Event ThisEvent;

    MyPriority = Priority;
    PWM_AddPins(PWM_PORTZ06);
    PWM_SetDutyCycle(PWM_PORTZ06, 500);

    // in here you write your initialization code
    // this includes all hardware and software initialization
    // that needs to occur.
    ES_Timer_InitTimer(TAPE_SENSOR_SERVICE_TIMER, TIMER_1_TICKS);
    // post the initial transition event
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
uint8_t PostTapeSensor(ES_Event ThisEvent)
{
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
ES_Event RunTapeSensor(ES_Event ThisEvent)
{
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    /********************************************
     in here you write your service code
     *******************************************/
    static ES_EventTyp_t lastEvent1 = NoFrontRightTape;
    static ES_EventTyp_t lastEvent2 = NoFrontLeftTape;
    static ES_EventTyp_t lastEvent3 = NoCannonTape;
    
    ES_EventTyp_t curEvent1 = NoFrontRightTape;
    ES_EventTyp_t curEvent2 = NoFrontLeftTape;
    ES_EventTyp_t curEvent3 = NoCannonTape;
    PWM_AddPins(PWM_PORTZ06);
    PWM_SetDutyCycle(PWM_PORTZ06, 500);
    
    uint16_t Tape3 = AD_ReadADPin(AD_PORTV6); //Read cannon tape
    uint16_t Tape1 = AD_ReadADPin(AD_PORTV4); //Read right tape sensor
    uint16_t Tape2 = AD_ReadADPin(AD_PORTV3);//Read left tape sensor

    switch (ThisEvent.EventType) {
    case ES_INIT:
        // No hardware initialization or single time setups, those
        // go in the init function above.
        //
        // This section is used to reset service for some reason
        break;

    case ES_TIMEOUT:
//        printf("Cannon Tape Value: %d\r\n", Tape3);
//        printf("Right Tape Value: %d\r\n", Tape1);
//        printf("Left Tape Value: %d\r\n", Tape2);
        if (Tape3 < 300) { 
            curEvent3 = CannonTape;
        }
        else{
            curEvent3 = NoCannonTape;   
        }
        if (Tape2 < 300) {
            curEvent2 = FrontLeftTape;
        }
        else{
            curEvent2 = NoFrontLeftTape;
        }
        if (Tape1 > 700) { 
            curEvent1 = FrontRightTape;
        }
        else{
            curEvent1 = NoFrontRightTape;
        }    
        if (curEvent3 != lastEvent3) { // check for change from last time
            ReturnEvent.EventType = curEvent3;
            ReturnEvent.EventParam = Tape3;
            lastEvent3 = curEvent3; // update history
            PostRobotHSM(ReturnEvent);
        }
        if (curEvent1 != lastEvent1) { // check for change from last time
            ReturnEvent.EventType = curEvent1;
            ReturnEvent.EventParam = Tape1;
            lastEvent1 = curEvent1; // update history
            PostRobotHSM(ReturnEvent);
        }
        if (curEvent2 != lastEvent2) { // check for change from last time
            ReturnEvent.EventType = curEvent2;
            ReturnEvent.EventParam = Tape2;
            lastEvent2 = curEvent2; // update history
            PostRobotHSM(ReturnEvent);
        }
        ES_Timer_InitTimer(TAPE_SENSOR_SERVICE_TIMER, TIMER_1_TICKS);
/*#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
            PostGenericService(ReturnEvent);
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

