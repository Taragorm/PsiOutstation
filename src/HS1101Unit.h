#ifndef HS1101UNIT_H
#define HS1101UNIT_H

#include "Telemetry.h"

//================================================================
template<
    uint8_t ADCPIN,
    typename TCOMPUTE,
    typename TSENSOR
    >
class HS1101Unit : TCOMPUTE, TSENSOR
{
    /// Adjust the temp by this
    int16_t _rawTAdj;

public:
    //--------------------------------------------------------------
    HS1101Unit()
    : _rawTAdj(0)
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

        TSENSOR::init();
    }
    //--------------------------------------------------------------
    void setup(float tempDeadband, float humidDeadband)
    {
        telemetry::Temperature.deadband.f  = tempDeadband;
        telemetry::Humidity.deadband.f     = humidDeadband;
    }
    //--------------------------------------------------------------
    void tAdjust(float f)
    {
        _rawTAdj = (int16_t)(f*TCOMPUTE::_therm_table_scale+0.5);
    }
    //--------------------------------------------------------------
    void read(bool force)
    {
        auto tcounts = analogRead(ADCPIN);
        auto tempRaw = TCOMPUTE::rawTemp(tcounts) + _rawTAdj;

        TSENSOR::beginCounting();
        // will idle-sleep here
        auto hcounts = TSENSOR::endCounting();

        int16_t humidRaw;
        /*auto status = */ TCOMPUTE::computeRH(hcounts, tempRaw, humidRaw);

        telemetry::Temperature.set( (float)TCOMPUTE::scaleTemp(tempRaw), force );
        telemetry::Humidity.set( (float)TCOMPUTE::scaleHumid(humidRaw), force );
    }
    //--------------------------------------------------------------
};
//================================================================

#endif
