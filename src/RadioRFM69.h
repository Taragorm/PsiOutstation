#ifndef RADIO_H
#define RADIO_H

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
			uint8_t IRQ_PIN
			>
class RadioRfm69
{
	inline static uint16_t _id;
    inline static uint8_t _freq;
    inline static uint16_t _nodeid;
    inline static uint16_t _netid;
	inline static psiiot::Buffer<BUFFSIZE> _buff;
	inline static RFM69_ATC _radio {CS_PIN, IRQ_PIN, /*isRFM69HW=*/ true, /*SPI*/ nullptr };
    inline static bool _requireAck;

public:
    //--------------------------------------------------------------------------------------
    static void setup()
    {
    }

    //--------------------------------------------------------------------------------------
	static void setup(
                        uint16_t stationID,
                        uint16_t nodeid,
                        uint8_t freq,
                        uint16_t netid
                        )
	{
        _id = stationID;
        _freq = freq;
        _nodeid = nodeid;
        _netid = netid;
	}

    //--------------------------------------------------------------------------------------
	static void send()
	{
        using namespace telemetry;

        if(!Telemetry::resetDirty())
            return;

        //
        // No need to wake the radio unless we're going
        // to send something
        _radio.initialize(_freq, _nodeid, _netid);
        _radio.setMode(RF69_MODE_RX); // oddly, we have to be in RX mode to send...

        _buff.reset();
        _buff.putID(_id);

        uint8_t cause = 0;
        //
        // load up
        //buff_.put(IOT_ENC('H'), humid);
        Telemetry* tp = _telemetry;
        for( auto i=0; i<(int)TelemIndex::DIMENSION; ++i, ++tp)
        {
            if(tp->state != TelemState::New)
                continue;

            _buff.put(tp->value);
            cause |= tp->cause;
        }

        _buff.put( 'C', cause);

        _buff.finalise();

        int bytes = _buff.writeSpaceUsed();
        bool ok;
        if(_requireAck)
        {
            ok = _radio.sendWithRetry(1, _buff.buffer(), bytes );
            _requireAck = false;
        }
        else
        {
            _radio.send(1, _buff.buffer(), bytes );
            ok = true;
        }

        ok = ok; // might want this later

        if (_radio.ACKRequested())
        {
            _radio.sendACK();
        }

        _radio.sleep();
	}
    //--------------------------------------------------------------------------------------
};


#endif