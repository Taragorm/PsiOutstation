#ifndef _PITSLEEP_H
#define _PITSLEEP_H

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <clocks.h>

#include "SleepControl.h"

template<unsigned SEC>
class PitSleep : public PinSleepControl
{
    typedef PinSleepControl Base;

public:
    //---------------------------------------------------
    static void setup()
    {
        // clock is shared with RTC
        if(!RtcControl::is32kClocked())
            RtcControl::clockLP32k();       // we assume 32k

        //RTC.PITINTCTRL = RTC_PI_bm;
        //RTC.PITCTRLA =
        //    RTC_PERIOD_CYC32768_gc
//            | 1 << RTC_PITEN_bp       	// Enable PIT with 1s rate
        //    ;
        
        RtcControl::pitPeriod(RtcControl::Period::CYC32K);
        RtcControl::clearPitInterruptFlags();
        RtcControl::enablePitInterrupt(true);
        RtcControl::enablePit(true);
    }

    //---------------------------------------------------
    static void sleep()
    {
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        for(unsigned n=0; n<SEC; ++n)
        {
            sleep_cpu(); //Go into sleep
            wdt_reset();            
        }            
        Base::sleep();
    }
    //---------------------------------------------------
};

#endif