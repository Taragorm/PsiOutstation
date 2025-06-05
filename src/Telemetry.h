#ifndef _TELEMETRY_H
#define _TELEMETRY_H

#include <stdint.h>
#include <psiiot.h>

namespace telemetry
{
enum class TelemState
{
    Cold,
    Old,
    New
};

enum class TelemType
{
    Real,
    Int32,
    Bool
};



enum class TelemIndex
{
    BattVolts,
    Temperature,
    Humidity,
    Pressure,
    Light,
    DIMENSION
};
//-----------------------------------------------------------
struct Telemetry
{
    TelemState state;
    const uint8_t cause;
    psiiot::UnpackedPoint<4> value;
    psiiot::UnpackedPoint<4>::ValueUnion scale;
    psiiot::UnpackedPoint<4>::ValueUnion deadband;
    
    static bool _dirty;
    
    //------------------------------------------------------
    Telemetry(char id, uint8_t acause)
    : cause(acause)
    {
        value.varNo = IOT_ENC(id);
    }
    //------------------------------------------------------            
    /**
     * Reset dirty flag, returning original state
     */
    static bool resetDirty()
    {
        auto ret = _dirty;
        _dirty = false;
        return ret;
    }        
    //------------------------------------------------------
    void set(float f, bool force);
    void set(int32_t i, bool force);
    //void set(bool b, bool force);
    void dump();
    static const char* State2Text(TelemState st);
    char varName() { return value.varName(); }
    char typeName() { return value.typeName(); }
        
};
//-----------------------------------------------------------
class TelemetryLogger
{
public:
    void setup() {}
    void init() {}
    void loop();    
    static void dump();
};    
//-----------------------------------------------------------

extern Telemetry _telemetry[(int)TelemIndex::DIMENSION];
//-----------------------------------------------------------
inline Telemetry& BattVolts{_telemetry[(int)TelemIndex::BattVolts]};
inline Telemetry& Temperature{_telemetry[(int)TelemIndex::Temperature]};
inline Telemetry& Humidity{_telemetry[(int)TelemIndex::Humidity]};
inline Telemetry& Pressure{_telemetry[(int)TelemIndex::Pressure]};
inline Telemetry& Light{_telemetry[(int)TelemIndex::Light]};

} // namespace
#endif