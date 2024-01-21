#include <I2CScanner.hpp>
#include <Logger.hpp>
#include <ESP8266WiFi.h>
#include <ESPAsyncUDP.h>
#include <AsyncTimer.h>
#include <Wire.h>

extern AsyncTimer asyncTimer;
_I2CScanner I2CScanner;

void _I2CScanner::setup()
{
	Wire.begin();
}

void _I2CScanner::start()
{
	if (intervalId)
	{
		return;
	}
	intervalId = asyncTimer.setInterval(std::bind(&_I2CScanner::scan, this), 5 * 1000);
}

void _I2CScanner::stop()
{
	if (!intervalId)
	{
		return;
	}
	asyncTimer.cancel(intervalId);
	intervalId = 0;
}

void _I2CScanner::scan() const
{
	LOG__DEBUG("Begin I2CScanner::scan");
	byte count = 0;
	for (byte i = 8; i < 120; i++)
	{
		Wire.beginTransmission(i);
		if (Wire.endTransmission() == 0)
		{
			LOG__DEBUG_F("I2CScanner::scan found address: %s (0x%s)", String(i, DEC).c_str(), String(i, HEX).c_str());
			count++;
		}
	}
	LOG__DEBUG_F("I2CScanner::scan found: %s device(s).", String(count, DEC).c_str());
	LOG__DEBUG("Begin I2CScanner::scan [done]");
}