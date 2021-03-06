/**
 * @file Dump.c
 * @author Alex VanScoy (vanscoy@utexas.edu), Prathik Srinivasan (prathik.srinivasan@utexas.edu) Jonathan Valvano, Matthew Yu
 * @TA Kyubum (Kevin) Shim
 * @section 17510
 * @brief 
 *    A set of debugging functions that capture data for later inspection.
 *    There are two primary methods:
 *       - DebugCapture will record data and time.
 *       - JitterMeasure will measure real time jitter.
 * @version 0.1
 * @date 2022-01-31
 *
 * @copyright Copyright (c) 2022
 */

/** File includes. */
#include <stdint.h>
#include "../inc/Dump.h"
#include "../inc/tm4c123gh6pm.h"

#define EXPECTEDPERIOD 78125

// Global variables
extern volatile uint32_t realTimeCount;
uint32_t DumpTimeBuffer[DUMPBUFSIZE];
uint32_t DumpDataBuf[DUMPBUFSIZE];
uint32_t DumpNum;
uint32_t timer1Value, timer1Prev, timer1Elapsed;
uint32_t RTMin, RTMax, RTJitter; //Global variables to store data for calculating jitter

void Timer1_Init(void) {
    volatile uint32_t delay;
    SYSCTL_RCGCTIMER_R    |= 0x02;                // 0) activate TIMER1
    delay                  = SYSCTL_RCGCTIMER_R;  // allow time to finish activating
    TIMER1_CTL_R           = 0x00000000;          // 1) disable TIMER1A during setup
    TIMER1_CFG_R           = 0x00000000;          // 2) configure for 32-bit mode
    TIMER1_TAMR_R          = 0x00000002;          // 3) configure for periodic mode, default down-count settings
    TIMER1_TAILR_R         = 0xFFFFFFFF;          // 4) reload value
    TIMER1_TAPR_R          = 0;                   // 5) bus clock resolution
    TIMER1_CTL_R           = 0x00000001;          // 10) enable TIMER1A
}

void DumpInit(void){
	Timer1_Init(); //Initialize Timer1
	DumpNum = 0; //Initialize DumpBuffer counter
}

void DumpCapture(uint32_t data){
    // Use TIMER1_TAR_R as current time
	if(SYSCTL_RCGCTIMER_R&0x02 && (DumpNum < DUMPBUFSIZE)) { // make sure timer 1 is active
		DumpDataBuf[DumpNum] = data;
		DumpTimeBuffer[DumpNum] = TIMER1_TAR_R; //Record data and time in appropriate buffers
		DumpNum+=1; //Update Dumpbuffer Counter
	}
}

uint32_t DumpCount(void){ 
    return DumpNum; //Returns counter for the DumpBuffer
}

uint32_t* DumpData(void){ 
    return DumpDataBuf;
}

uint32_t* DumpTime(void){ 
    return DumpTimeBuffer;
}

void JitterInit(void){
    /* TODO (EE445L Lab 2): complete this. */
	RTMax = RTJitter = 0;
	RTMin = 0xFFFFFFFF;
	Timer1_Init();
	// timer1Prev = TIMER1_TAR_R; // "Yeah, that's not gonna work" according to Jared
}

void JitterMeasure(void){
    /* TODO (EE445L Lab 2): complete this. */
	if(SYSCTL_RCGCTIMER_R&0x02) { // make sure timer 1 is active
		timer1Value = TIMER1_TAR_R;
		// have a flag for first meaure, using realTimeCount from Lab2.c
		if (realTimeCount > 1) {
		  timer1Elapsed = timer1Prev - timer1Value;	
	    if(timer1Elapsed < RTMin) RTMin = timer1Elapsed;
	    if(timer1Elapsed > RTMax) RTMax = timer1Elapsed; //Update max or min value based on elapsed time
		}
	  // RTJitter = RTMax - RTMin; // not needed
		timer1Prev = timer1Value;
	}
}

uint32_t JitterGet(void){
    /* TODO (EE445L Lab 2): complete this. */
    return RTMax - RTMin;
}




