#ifndef HS1101UNIT_H
#define HS1101UNIT_H

#include "Telemetry.h"

//================================================================
template<
    uint8_t ADCPIN,
    typename TCOMPUTE,
    typename TSENSOR,
    uint8_t OVERSAMPLE_BITS = 3
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
        telemetry::Temperature.deadband.f  = 1.0;
        telemetry::Humidity.scale.f        = 1.0;
        telemetry::Humidity.value.type     = psiiot::ptFloat;
        telemetry::Humidity.deadband.f     = 7;

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
        //
        // Oversample just to make the lower bits a touch more stable
        uint16_t tcounts = 0;
        for(uint8_t s=0; s< (1<<OVERSAMPLE_BITS); ++s)
        {
            if(s>0) delay(5);
            tcounts += analogRead(ADCPIN);
        }

        tcounts >>= OVERSAMPLE_BITS;

        //Serial.printf("read tadc=%u\r\n", tcounts);
        auto tempRaw = TCOMPUTE::rawTemp(tcounts) + _rawTAdj;

        //Serial.printf("read humid begin\r\n", tcounts);
        TSENSOR::beginCounting();
        // will idle-sleep here
        //Serial.printf("read humid begun\r\n", tcounts);
        auto hcounts = TSENSOR::endCounting();
        //Serial.printf("read sensor=%u\r\n", hcounts);

        int16_t humidRaw;
        /*auto status = */ TCOMPUTE::computeRH(hcounts, tempRaw, humidRaw);

        telemetry::Temperature.set( (float)TCOMPUTE::scaleTemp(tempRaw), force );
        telemetry::Humidity.set( (float)TCOMPUTE::scaleHumid(humidRaw), force );
    }
    //--------------------------------------------------------------
};
//================================================================

#endif
