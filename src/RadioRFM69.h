#ifndef RADIO_H
#define RADIO_H

#undef min
#undef max

#include <stdint.h>
#include <psiiot.h>
#include <RFM69.h>         //get it here: https://www.github.com/lowpowerlab/rfm69
#include <RFM69_ATC.h>     //get it here: https://www.github.com/lowpowerlab/rfm69
#include "Telemetry.h"


/**
 * Facet class for the outstation, implementing
 * TOUTPUT for the RFM69 radio
 */
template<   uint8_t BUFFSIZE,
			uint8_t CS_PIN,
			uint8_t IRQ_PIN,
            bool ISRFM69HW
			>
class RadioRfm69
{
public:
//    typedef std::unary_function<bool, void> ActiveCallback_t;
    typedef  void (*ActiveCallback_t)(bool);

private:    
	inline static uint16_t _id;
    inline static uint8_t _freq;
    inline static uint16_t _nodeid;
    inline static uint16_t _netid;
	inline static psiiot::Buffer<BUFFSIZE> _buff;
	inline static RFM69_ATC _radio {
                        CS_PIN, 
                        IRQ_PIN, 
                        /*isRFM69HW=*/ ISRFM69HW, 
                        /*SPI*/ nullptr 
                        };
    inline static bool _requireAck;
    inline static ActiveCallback_t _activeCallback = nullptr;
public:
    //--------------------------------------------------------------------------------------
    static void setup()
    {
    }
    //--------------------------------------------------------------------------------------
    static void setCallback(ActiveCallback_t cb)
    {
        _activeCallback = cb;
    }
    //--------------------------------------------------------------------------------------
    static ActiveCallback_t getCallback() { return _activeCallback; }
    //--------------------------------------------------------------------------------------
	static void setup(
                        uint16_t stationID,
                        uint16_t nodeid,
                        uint8_t freq,
                        uint16_t netid,
                        ActiveCallback_t cb
                        )
	{
        _id = stationID;
        _freq = freq;
        _nodeid = nodeid;
        _netid = netid;
        _activeCallback = cb;
	}
    //--------------------------------------------------------------------------------------
    static bool isNewTelemetry()
    {
        using namespace telemetry;

        Telemetry* tp = _telemetry;
        for( auto i=0; i<(int)TelemIndex::DIMENSION; ++i, ++tp)
        {
            if(tp->state == TelemState::New)
                return true;
        }
        return false;
    }
    //--------------------------------------------------------------------------------------
	static void send()
	{
        using namespace telemetry;

        if(!isNewTelemetry())
        {
            //Serial.println("No telemetry change");
            return;
        }

        //
        // No need to wake the radio unless we're going
        // to send something
        bool ok =  _radio.initialize(_freq, _nodeid, _netid);
        //Serial.printf("radio Fr=%d, Nd=%d Nt=%d\r\n", _freq, _nodeid, _netid);
        if(!ok)
        {
            Serial.println("Radio init fail!");
            return;
        }
#ifdef RADIO_TX_POWER
        _radio.setPowerLevel(RADIO_TX_POWER);
#endif

#if IS_RFM69HW_HCW
        _radio.setHighPower(); //must include this only for RFM69HW/HCW!
#endif

#ifdef RADIO_ENCRYPTKEY
        _radio.encrypt(RADIO_ENCRYPTKEY);
#endif
        //_radio.setMode(RF69_MODE_RX); // oddly, we have to be in RX mode to send...

        //_radio.readAllRegs();

        _buff.reset();
        _buff.putID(_id);

        uint8_t cause = 0;
        uint8_t pointCount = 0;

        //
        // load up
        //buff_.put(IOT_ENC('H'), humid);
        Telemetry* tp = _telemetry;
        for( auto i=0; i<(int)TelemIndex::DIMENSION; ++i, ++tp)
        {
            if(tp->state != TelemState::New)
                continue;

            //Serial.print('#');
            _buff.put(tp->value);
            cause |= tp->cause;
            ++pointCount;
        }

        if(!pointCount)
        {
            //Serial.println("NO DATA");
            return;
        }

        _buff.put( 'C', cause);

        _buff.finalise();

        int bytes = _buff.writeSpaceUsed();

        if(_activeCallback) 
            _activeCallback(true);

        if(_requireAck)
        {
            //Serial.printf("SEND+ACK %d\r\n", bytes);
            ok = _radio.sendWithRetry(GW_NODEID, _buff.buffer(), bytes );
            _requireAck = false;
        }
        else
        {
            //Serial.printf("SEND %d\r\n", bytes);
            _radio.send(GW_NODEID, _buff.buffer(), bytes );
            ok = true;
        }

        ok = ok; // might want this later

        if (_radio.ACKRequested())
        {
            _radio.sendACK();
        }

        if(_activeCallback) 
            _activeCallback(false);

        _radio.sleep();
	}
    //--------------------------------------------------------------------------------------
};


#endif