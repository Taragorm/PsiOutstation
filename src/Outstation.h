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
#include <Telemetry.h>
#include <psiutil.h>

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
class NullOutputFacet : public NullFacet
{
public:    
    void setup() {}
    void setup(
                        uint16_t stationID,
                        uint16_t nodeid,
                        uint8_t freq,
                        uint16_t netid
                        )    
    {}
    void send() {}
};
//=======================================================
class NullSensorFacet : public NullFacet
{
public:    
    void setup() {}
    void read(bool) {}
};
//=======================================================
template<unsigned SEC>
class DelaySleep 
{
public:
    void setup() {}

    void sleep()
    {
        for(int i=0; i<SEC; ++i)
            delay(1000);
    }

    void setupSleepPinStates(const void*)
    {}

    void wake() {}
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
template<uint8_t PIN, bool DEF>
class ControlPin
{
public:
    const bool OFF = DEF;
    const bool ON  = !DEF;
    constexpr bool implemented() { return true; }
    //---------------------------------------------------
    void setup()
    {
        pinMode(PIN, OUTPUT);
        digitalWriteFast(PIN,DEF);
        //digitalWrite(PIN,DEF);
    }

    //---------------------------------------------------
    /**
     * Sets the REAL state of the pin
     */
    void set(bool st=true)
    {
        digitalWriteFast(PIN,st);
    }
    //---------------------------------------------------
    /**
     * Set the state of the pin vs the defined default.
     * 
     */
    void setLogical(bool st=true)
    {
        //Serial.printf("Setting %d = log %d\r\n", PIN, st); delay(20);
        if(st)
            digitalWriteFast(PIN, ON );
        else
            digitalWriteFast(PIN, OFF );
        //digitalWrite(PIN, DEF ? !st : st );
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
    bool def() const { return DEF; }
    //---------------------------------------------------
    void delay(uint32_t ms)
    {
        ::delay(ms);
    }
};
//=======================================================
/**
 * Facet to look like a control pin
 */
template<uint8_t PIN, bool DEF>
class NullControlPin
{
public:
    constexpr bool implemented() { return false; }
    //---------------------------------------------------
    void setup()
    {
    }

    //---------------------------------------------------
    /**
     * Sets the REAL state of the pin
     */
    void set(bool st=true)
    {
    }
    //---------------------------------------------------
    /**
     * Set the state of the pin vs the defined default.
     * 
     */
    void setLogical(bool st=true)
    {
    }
    //---------------------------------------------------
    void reset()
    {
    }
    //---------------------------------------------------
    void toggle()
    {
    }
    //---------------------------------------------------
    bool def() const { return DEF; }
    //---------------------------------------------------
    void delay(uint32_t ms) {}
    //---------------------------------------------------
};
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
{
    inline static uint8_t _sendAllInterval;
    inline static uint8_t _sendAllCount;

public:
    typedef TSLEEP sleep_t;
    typedef TCLIMATE climate_t;
    typedef TBATT batt_t;
    typedef TLIGHT light_t;
    typedef TOUTPUT output_t;
    typedef TPOWERPIN powerpin_t;
    typedef TLED led_t;

    sleep_t sleep;
    climate_t climate;
    batt_t battery;
    light_t lightsensor;
    powerpin_t powerpin;
    led_t ledpin;
    output_t output;
    //---------------------------------------------------
    IotStation()
    {
    }
    //---------------------------------------------------
    void led(bool st)
    {
        ledpin.setLogical(st);
    }
    //---------------------------------------------------
    void setup(uint8_t sendAllInterval=8)
    {
        _sendAllInterval = sendAllInterval;
        XTRACEC('1');
        climate.setup();
        XTRACEC('2');
        lightsensor.setup();
        ledpin.setup();
        powerpin.setup();
        sleep.setup();
        battery.setup();
        output.setup();
        XTRACEC('9');

    }
    //---------------------------------------------------
    void loop()
    {
        wdt_reset(); 
        powerpin.setLogical(true);       
        powerpin.delay(50);
        sleep.wake();

        // Every so often send everything
        bool force = false;
        if(_sendAllCount==0)
        {
            force = true;
            _sendAllCount = _sendAllInterval-1;
        }
        else
            --_sendAllCount;

        battery.read(force);
        lightsensor.read(force);
        climate.read(force);
        output.send();
        delay(10);
        powerpin.setLogical(false);       
        sleep.sleep();
    }
    //---------------------------------------------------
    void toggleLed()
    {
        ledpin.toggle();
    }
    //---------------------------------------------------
    void powerSwitch(bool st)
    {
        powerpin.setLogical(st);
    }
    //---------------------------------------------------
    void dumpTelemetry()
    {
        telemetry::TelemetryLogger::dump();
    }
    //---------------------------------------------------
};
//=======================================================



#endif /* OUTSTATION_H_ */