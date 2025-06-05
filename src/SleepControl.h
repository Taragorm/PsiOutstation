#ifndef SLEEP_CONTROL_h
#define SLEEP_CONTROL_h

struct SleepStates
{
    uint8_t pin;
    uint8_t modes[2];
};

class PinSleepControl
{
    inline static const SleepStates* _states;
public:    
    static void setupSleepPinStates(const SleepStates* states)
    {
        _states = states;
    }        
    //--------------------------------------------------------------
    static void sleep()
    {
        if(!_states)
            return;
            
        for(auto wp = _states; wp->pin < 255; ++wp)
            pinMode(wp->pin, wp->modes[1]);        
    }
            
    //--------------------------------------------------------------
    static void wake()
    {
        if(!_states)
            return;
            
        for(auto wp = _states; wp->pin < 255; ++wp)
            pinMode(wp->pin, wp->modes[0]);
    }        
    //--------------------------------------------------------------
};    



#endif
