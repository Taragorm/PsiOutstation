/*
 * Outstation.h
 *
 * Created: 22/02/2025 16:44:05
 *  Author: Simon
 */


#ifndef OUTSTATION_H_
#define OUTSTATION_H_

#include <stdint.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <clocks.h>

//=======================================================
class NullFacet
{
public:
    constexpr bool implemented() { return false; }
    void setup() {}
    void poll() {}
    void sleep() {}
    void unsleep() {}
    void set() {}
    void reset() {}
    void toggle() {}
    void loop() {}
};


//=======================================================
//class HS1101
//{
//public:
    //constexpr bool gotPressure() { return false; }
    //constexpr bool gotTemperature() { return true; }
    //constexpr bool gotHumidity(); { return true; }
    //void setup()
    //{
    //}
//};
//=======================================================
/**
 * Facet to control a power pin
 */
template<uint8_t PIN>
class ControlPin
{
public:
    constexpr bool implemented() { return true; }
    //---------------------------------------------------
    void setup()
    {
        digitalWriteFast(PIN,0);
        pinMode(PIN, OUTPUT);
    }

    //---------------------------------------------------
    void set(bool st=true)
    {
        digitalWriteFast(PIN,st);
    }

    //---------------------------------------------------
    void reset()
    {
        digitalWriteFast(PIN,0);
    }
    //---------------------------------------------------
    void toggle()
    {
        digitalWriteFast(PIN, !digitalReadFast(PIN));
    }
    //---------------------------------------------------
};
//=======================================================
//=======================================================
/**
 * Templated class for an outstation that uses facet
 * classes
 */
template<
        typename TCLIMATE,
        typename TBATT,
        typename TLIGHT,
        typename TLED,
        typename TPOWERPIN,
        typename TSLEEP,
        typename TOUTPUT
        >
class IotStation
    :   public TCLIMATE,
        public TLIGHT,
        public TLED,
        public TPOWERPIN,
        public TSLEEP,
        public TBATT,
        public TOUTPUT
{
    inline static uint8_t _sendAllInterval;
    inline static uint8_t _sendAllCount;

public:
    typedef TSLEEP sleep_t;
    typedef TCLIMATE climate_t;
    typedef TBATT batt_t;
    typedef TLIGHT light_t;
    typedef TOUTPUT output_t;

    //---------------------------------------------------
    IotStation()
    {
    }

    //---------------------------------------------------
    void setup(uint8_t sendAllInterval=8)
    {
        _sendAllInterval = sendAllInterval;

        TCLIMATE::setup();
        TLIGHT::setup();
        TLED::setup();
        TPOWERPIN::setup();
        TSLEEP::setup();
        TBATT::setup();
        TOUTPUT::setup();
    }

    //---------------------------------------------------
    void loop()
    {
        wdt_reset();        
        TSLEEP::wake();
        TLED::toggle();

        // Every so often send everything
        bool force = false;
        if(_sendAllCount==0)
        {
            force = true;
            _sendAllCount = _sendAllInterval-1;
        }
        else
            --_sendAllCount;

        TBATT::read(force);
        TLIGHT::read(force);
        TCLIMATE::read(force);

        TOUTPUT::send();

        TSLEEP::sleep();
    }
    //---------------------------------------------------
    void toggleLed()
    {
        TLED::toggle();
    }
    //---------------------------------------------------
    void led(bool st)
    {
        TLED::set(st);
    }
    //---------------------------------------------------
    void powerSwitch(bool st)
    {
        TPOWERPIN::set(st);
    }
    //---------------------------------------------------
    batt_t& vbatt() { return (TBATT&)*this; }
    //---------------------------------------------------
    light_t& light() { return (TLIGHT&)*this; }
    //---------------------------------------------------
    climate_t& climate() { return (TCLIMATE&)*this; }
    //---------------------------------------------------
    output_t& output() { return (TOUTPUT&)*this; }
    //---------------------------------------------------
    sleep_t& sleepController() { return (TSLEEP&)*this; }
    //---------------------------------------------------
    void dumpTelemetry()
    {
        telemetry::TelemetryLogger::dump();
    }
    //---------------------------------------------------
};
//=======================================================



#endif /* OUTSTATION_H_ */