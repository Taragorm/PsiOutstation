#include "Telemetry.h"
#include <math.h>
#include <stdlib.h>
#include <Arduino.h>

using namespace psiiot;

namespace telemetry
{
    
Telemetry _telemetry[(int)TelemIndex::DIMENSION]
{
//   VN   CAUSE [some seem to be missing..]
    {'V', 1<<1}, 
    {'T', 1<<2}, 
    {'H', 1<<4}, 
    {'P', 1<<3}, 
    {'L', 1<<5}
};

bool Telemetry::_dirty;

//----------------------------------------------------------------
//void Telemetry::set(bool b, bool force)
//{
    //if(force || value.value. != b || state == TelemState::Cold) 
    //{
        //_dirty = true;
        //value.b = b;
        //state = TelemState::New;
    //} 
    //else
    //{
        //state = TelemState::Old;        
    //}        
//}

//----------------------------------------------------------------
void Telemetry::set(float f, bool force)
{
    f *= scale.f;
    
    if(force || state == TelemState::Cold || fabs(f-value.value.f) >= deadband.f )
    {
        _dirty = true;
        value.value.f = f;
        state = TelemState::New;
    }
    else
    {
        state = TelemState::Old;
    }
    
}    
//----------------------------------------------------------------
void Telemetry::set(int32_t i, bool force)
{
    if(force || state == TelemState::Cold || abs(i-value.value.i32) >= deadband.i32 )
    {
        _dirty = true;
        value.value.i32 = i;
        state = TelemState::New;
    }
    else
    {
        state = TelemState::Old;
    }    
}    
//----------------------------------------------------------------
const char* Telemetry::State2Text(TelemState st)
{
    switch(st)
    {
        case TelemState::Cold: return "Cld";
        case TelemState::New: return "New";
        case TelemState::Old: return "Old";
    }        
    return "???";
}    
//----------------------------------------------------------------
void Telemetry::dump()
{
    Serial.printf("%c %s ", varName(), State2Text(state) );
    switch(value.type)
    {
        case ptFloat: 
            Serial.printf("%f\r\n", (double)value.value.f ); 
            break;

        case ptInt32:
            Serial.printf("%ld\r\n", value.value.i32 ); 
            break;
            
        default:
            Serial.printf("Unk Type %d\r\n", value.type );
            break;
    }        
}    
//----------------------------------------------------------------
//----------------------------------------------------------------
void TelemetryLogger::dump()
{
    Telemetry* tp = _telemetry;
    for( auto i=0; i<(int)TelemIndex::DIMENSION; ++i, ++tp)
    {
        if(tp->state != TelemState::Cold)
        tp->dump();
    }  
}    
//----------------------------------------------------------------
void TelemetryLogger::loop()
{
  dump();        
}    
//----------------------------------------------------------------
}