#ifndef BME280UNIT_H
#define BME280UNIT_H

#include "Telemetry.h"
#include "SparkFunBME280.h"

//================================================================
template<
    uint8_t CSPIN
    >
class BME280Unit
{
    BME280 _bme;
    float _tempOffset;
public:
    //--------------------------------------------------------------
    BME280Unit()
    : _bme()
    {}
    //--------------------------------------------------------------
    void setup()
    {
        telemetry::Temperature.scale.f     = 1.0;
        telemetry::Temperature.value.type  = psiiot::ptFloat;
        telemetry::Temperature.deadband.f  = 0.1;
        telemetry::Humidity.scale.f        = 1.0;
        telemetry::Humidity.value.type     = psiiot::ptFloat;
        telemetry::Humidity.deadband.f     = 1;
        telemetry::Pressure.scale.f        = 1.0;
        telemetry::Pressure.value.type     = psiiot::ptFloat;
        telemetry::Pressure.deadband.f     = 10;

    }
    //--------------------------------------------------------------
    void setup(float tempDeadband, float humidDeadband)
    {
        telemetry::Temperature.deadband.f  = tempDeadband;
        telemetry::Humidity.deadband.f     = humidDeadband;
    }
    //--------------------------------------------------------------
    void setTempOffset(float tempOffset)
    {
        _tempOffset =  tempOffset;
    }
    //--------------------------------------------------------------
    void tAdjust(float f)
    {
        //_rawTAdj = (int16_t)(f*TCOMPUTE::_therm_table_scale+0.5);
    }
    //--------------------------------------------------------------
    void read(bool force)
    {
        _bme.beginSPI(CSPIN);    
        float t = _bme.readTempC() + _tempOffset;
        float h = _bme.readFloatHumidity();
        float p = _bme.readFloatPressure();
        XTRACEF2("t=%d,h=%d,p=%ul\r\n", (int)(t*10), (int)h, (unsigned long)p);
        telemetry::Temperature.set( t, force );
        telemetry::Humidity.set( h, force );
        telemetry::Pressure.set( p, force );
    }
    //--------------------------------------------------------------
};
//================================================================

#endif
