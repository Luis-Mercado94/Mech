/*
 * File:   TestHarness.c
 * Author: mlira2
 *
 * Created on October 1, 2019, 12:57 AM
 */


#include <xc.h>
#include <BOARD.h>
#include <roach.h>
#include <AD.h>

#include <serial.h>
#include <stdio.h>




//


// here we can define our constants
#define FLEFT_BUMP_MASK (1)
#define FRIGHT_BUMP_MASK (1<<1)
#define RLEFT_BUMP_MASK (1<<2)
#define RRIGHT_BUMP_MASK (1<<3)


#define DELAY(x)    for (wait = 0; wait <= x; wait++) {asm("nop");}
#define A_BIT       18300
#define A_BIT_MORE  36600
#define YET_A_BIT_LONGER (A_BIT_MORE<<2)
#define A_LOT       183000
#define NUM_TIMES_REPEAT_LED 5
#define NUM_TIMES_REPEAT_LED 5
#define NUMLEDS 12
#define MOTOR_TIME (A_LOT<<2)

#define LOW_BAT 263
#define HIGH_BAT 310

#define LIGHT_THRESHOLD 470
#define DARK_THRESHOLD 530
static uint16_t lastLight = 0; 


#define ROACH_BAT_VOLTAGE BAT_VOLTAGE

int main(void) {
    
    unsigned int wait;
    unsigned i, j, ass;
    int speed, speed2;
    unsigned int batteryValue, light;
    
    //Roach_Init();

    //Roach_Init();
    BOARD_Init();
    Roach_Init();
      
    
    //Roach_LeftMtrSpeed(80);
    
    while(1) {
    
        
        
//        printf("\r\nZa warudo!");
        //Roach_LeftMtrSpeed(80);
        //Roach_LEDSSet(0b111111111111);
        
        switch(Roach_ReadBumpers()){ 
    
    
    
            case FLEFT_BUMP_MASK: // Front Left Bumper 
               
                    for (i = 0; i < NUM_TIMES_REPEAT_LED; i++) {
                        for (j = 0; j < NUMLEDS; j++) {
                            Roach_LEDSSet(1 << j);
                            DELAY(A_BIT_MORE);
                        }
                        Roach_LEDSSet(0);
                        DELAY(A_BIT_MORE);
                    }
                    speed = 100;
                    printf("\r\nLeft Motor at %d", speed);
                    Roach_LeftMtrSpeed(speed);
                    Roach_RightMtrSpeed(speed);
                    DELAY(MOTOR_TIME);
                    speed -= 50;
                    printf("\r\nLeft Motor at %d", speed);
                    Roach_LeftMtrSpeed(speed);
                    Roach_RightMtrSpeed(speed);
                    DELAY(MOTOR_TIME);
                    speed -= 50;
                    printf("\r\nLeft Motor at %d", speed);
                    Roach_LeftMtrSpeed(speed);
                    Roach_RightMtrSpeed(speed);
                    DELAY(MOTOR_TIME);
                    speed -= 50;
                    printf("\r\nLeft Motor at %d", speed);
                    Roach_LeftMtrSpeed(speed);
                    Roach_RightMtrSpeed(speed);
                    DELAY(MOTOR_TIME);
                    speed -= 50;
                    printf("\r\nLeft Motor at %d", speed);
                    Roach_LeftMtrSpeed(speed);
                    Roach_RightMtrSpeed(speed);
                    DELAY(MOTOR_TIME);
                    speed = 0;
                    Roach_LeftMtrSpeed(speed);
                    Roach_RightMtrSpeed(speed);
                    printf("\r\nMotor test complete ");
                    break;

    
            case FRIGHT_BUMP_MASK: // Front Right Bumper   
                
                speed = 100;
                speed2 = -100;
                printf("\r\nLeft Motor at %d", speed);
                printf("\r\nRight Motor at %d", speed2);
                Roach_LeftMtrSpeed(speed);
                Roach_RightMtrSpeed(speed2);
                DELAY(MOTOR_TIME);
                speed = 0;
                speed2 = 0;
                printf("\r\nLeft Motor at %d", speed);
                printf("\r\nRight Motor at %d", speed2);
                Roach_LeftMtrSpeed(speed);
                Roach_RightMtrSpeed(speed2);
                DELAY(MOTOR_TIME);
                speed = -100;
                speed2 = 100;
                printf("\r\nLeft Motor at %d", speed);
                printf("\r\nRight Motor at %d", speed2);
                Roach_LeftMtrSpeed(speed);
                Roach_RightMtrSpeed(speed2);
                DELAY(MOTOR_TIME);
                speed = 0;
                speed2 = 0;
                printf("\r\nLeft Motor at %d", speed);
                printf("\r\nRight Motor at %d", speed2);
                Roach_LeftMtrSpeed(speed);
                Roach_RightMtrSpeed(speed2);
                printf("\r\nTurning Test complete, suck my ass");
                
                break;
    
            case RLEFT_BUMP_MASK: // Rear Left Bumper, light sensor
//                FlashLEDBar(2);
                DELAY(A_LOT);
                for (ass = 0; ass < A_LOT >> 2; ass++) {
                    light = Roach_LightLevel();
                    if (ass % 10000 == 0) {
                        printf("\r\nCurrent Light Level: %d", light);
                    }
                    light *= 12;
                    light /= 1023;
                    Roach_BarGraph(light);
                }
                printf("\r\nLight Level Test Complete");
                DELAY(A_LOT);
                break;
//                FlashLEDBar(2);
    
            case RRIGHT_BUMP_MASK: // Rear Right Bumper , battery level 
                
                //Roach_LEDSSet(0xFFF);
                batteryValue = Roach_BatteryVoltage();
                printf("\r\nBattery voltage is %d", batteryValue);
                batteryValue -= LOW_BAT;
                batteryValue *= 12;
                batteryValue /= (HIGH_BAT - LOW_BAT);
                Roach_BarGraph(batteryValue);
                printf("\r\nBattery Level Test Complete");
                DELAY(A_LOT << 2);
                
                //DELAY(A_BIT);
                break;
                
        }
    
    }
    
    //while(1);
    //return 0;
}
