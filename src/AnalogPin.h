#ifndef ANALOGPIN_H
#define ANALOGPIN_H

#include <stdint.h>
#include <psiiot.h>
#include "Telemetry.h"

//==================================================================
class NullAnalog
{
public:
    //--------------------------------------------------------------
    void setup()
    {
    }
    //--------------------------------------------------------------
    void setup(float, float)
    {
    }
    //--------------------------------------------------------------
    void read(bool)
    {
    }
    //--------------------------------------------------------------
};
//==================================================================
template<uint8_t PIN, telemetry::TelemIndex TELEMIX>
class AnalogPin
{
    telemetry::Telemetry& TELEM;
    
public:
    AnalogPin()
    : TELEM{telemetry::_telemetry[(int)TELEMIX]}
    {
    }        
    //--------------------------------------------------------------
    void setup()
    {
        TELEM.scale.f       = 1.0;
        TELEM.value.type    = psiiot::ptFloat;
    }
    //--------------------------------------------------------------
    void setup(float scale, float deadband)
    {
        TELEM.scale.f       = scale;
        TELEM.deadband.f    = deadband;
    }
    //--------------------------------------------------------------
    void read(bool force)
    {
        TELEM.set( (float) analogRead(PIN), force);
    }
    //--------------------------------------------------------------
};
//==================================================================

#endif
