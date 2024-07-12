#include "Logger.hpp"
#include <Arduino.h>
#include <string.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <TimeLib.h>

_Logger Logger(&Serial);

void _Logger::setup()
{
	stream->flush();
	Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
	Serial.setDebugOutput(true);
}

void _Logger::log(const char *p1, const char *p2, const char *p3) const
{
	stream->print(p1);
	stream->print(p2);
	if (p3)
	{
		stream->print(p3);
	}
	stream->println();
}

void _Logger::log(const __FlashStringHelper *p1, const __FlashStringHelper *p2, const __FlashStringHelper *p3) const
{
	stream->print(p1);
	stream->print(p2);
	if (p3)
	{
		stream->print(p3);
	}
	stream->println();
}

void _Logger::log(const __FlashStringHelper *p1, const char *p2, const char *p3) const
{
	stream->print(p1);
	stream->print(p2);
	if (p3)
	{
		stream->print(p3);
	}
	stream->println();
}

void _Logger::logf(const char *p1, const char *format, ...) const
{
	stream->print(p1);

	va_list arg;
	va_start(arg, format);
	char temp[256];
	char *buffer = temp;
	size_t len = vsnprintf(temp, sizeof(temp), format, arg);
	va_end(arg);
	if (len > sizeof(temp) - 1)
	{
		buffer = new char[len + 1];
		if (!buffer)
		{
			return;
		}
		va_start(arg, format);
		vsnprintf(buffer, len + 1, format, arg);
		va_end(arg);
	}
	len = stream->write((const uint8_t *)buffer, len);
	if (buffer != temp)
	{
		delete[] buffer;
	}
	stream->println();
}

void _Logger::logf(const __FlashStringHelper *p1, const char *format, ...) const
{
	stream->print(p1);

	va_list arg;
	va_start(arg, format);
	char temp[256];
	char *buffer = temp;
	size_t len = vsnprintf(temp, sizeof(temp), format, arg);
	va_end(arg);
	if (len > sizeof(temp) - 1)
	{
		buffer = new char[len + 1];
		if (!buffer)
		{
			return;
		}
		va_start(arg, format);
		vsnprintf(buffer, len + 1, format, arg);
		va_end(arg);
	}
	len = stream->write((const uint8_t *)buffer, len);
	if (buffer != temp)
	{
		delete[] buffer;
	}
	stream->println();
}

void ICACHE_FLASH_ATTR _Logger::writeEvent(String type, String src, String desc, String data) {
	JsonDocument root;
	root["type"] = type;
	root["src"] = src;
	root["desc"] = desc;
	root["data"] = data;
	root["time"] = now();
	File eventlog = LittleFS.open("/eventlog.json", "a");
	serializeJson(root, eventlog);
	eventlog.print("\n");
	eventlog.close();
}

void ICACHE_FLASH_ATTR _Logger::writeLatest(String uid, String username, int acctype) {
	JsonDocument root;
	root["uid"] = uid;
	root["username"] = username;
	root["acctype"] = acctype;
	root["timestamp"] = now();
	File latestlog = LittleFS.open("/latestlog.json", "a");
	serializeJson(root, latestlog);
	latestlog.print("\n");
	latestlog.close();
}