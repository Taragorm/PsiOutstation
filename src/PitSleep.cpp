#include "PitSleep.h"

#ifndef CUSTOM_PIT_VECTOR
/**
 * PIT Interrupt vector
 */
ISR(RTC_PIT_vect)
{
    if( (RTC.PITINTCTRL & 1) && (RTC.PITINTFLAGS & 1) )
    {
    }
    RTC.PITINTFLAGS = 1;
    //Serial.print(RTC.PITINTFLAGS ? "$" : ".");
}
#endif