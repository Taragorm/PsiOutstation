#ifndef ANALOGPIN_H
#define ANALOGPIN_H

#include <stdint.h>
#include <psiiot.h>
#include "Telemetry.h"

//==================================================================
template<uint8_t PIN, telemetry::TelemIndex TELEMIX>
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
    telemetry::Telemetry  * const TELEM;
    
public:
    AnalogPin()
    : TELEM{telemetry::_telemetry+(int)TELEMIX}
    {
    }        
    //--------------------------------------------------------------
    void setup()
    {
        TELEM->scale.f       = 1.0;
        TELEM->deadband.f    = 0.0;
        TELEM->value.type    = psiiot::ptFloat;
    }
    //--------------------------------------------------------------
    void setup(float scale, float deadband)
    {
        TELEM->scale.f       = scale;
        TELEM->deadband.f    = deadband;
    }
    //--------------------------------------------------------------
    void read(bool force)
    {
        auto v = analogRead(PIN);
        //XTRACEF2("rd=%u\r\n", v);
        TELEM->set( (float)v, force);
    }
    //--------------------------------------------------------------
};
//==================================================================

#endif
