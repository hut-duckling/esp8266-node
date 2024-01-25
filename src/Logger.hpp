#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <WString.h>
#include <Stream.h>
#include <stdio.h>
#include <HardwareSerial.h>

#define STR_INDIR(x) #x
#define STR(x) STR_INDIR(x)
#define LOGPREFIX __FILE__ ":" STR(__LINE__) "  "

#ifdef LOGGER_ENABLE_TRACE_LOG
#define LOG__TRACE(str...) Logger.log(F(LOGPREFIX "[TRACE] "), str);
#define LOG__TRACE_F(format, str...) Logger.logf(F(LOGPREFIX "[TRACE] "), format, str);
#else
#define LOG__TRACE(str...)
#define LOG__TRACE_F(format, str...)
#endif

#ifdef LOGGER_ENABLE_DEBUG_LOG
#define LOG__DEBUG(str...) Logger.log(F(LOGPREFIX "[DEBUG] "), str);
#define LOG__DEBUG_F(format, str...) Logger.logf(F(LOGPREFIX "[DEBUG] "), format, str);
#else
#define LOG__DEBUG(str...)
#define LOG__DEBUG_F(format, str...)
#endif

#define LOG__INFO(str...) Logger.log(F(LOGPREFIX "[INFO] "), str);
#define LOG__INFO_F(format, str...) Logger.logf(F(LOGPREFIX "[INFO] "), format, str);

#define LOG__WARN(str...) Logger.log(F(LOGPREFIX "[WARN] "), str);
#define LOG__WARN_F(format, str...) Logger.logf(F(LOGPREFIX "[WARN] "), format, str);

#define LOG__ERROR(str...) Logger.log(F(LOGPREFIX "[ERROR] "), str);
#define LOG__ERROR_F(format, str...) Logger.logf(F(LOGPREFIX "[ERROR] "), format, str);

class _Logger
{
public:
	_Logger(Stream *stream): stream(stream) {
	}

	void setup();

	void log(const char *p1, const char *p2, const char *p3 = nullptr) const;
	void log(const __FlashStringHelper *p1, const char *p2, const char *p3 = nullptr) const;
	void log(const __FlashStringHelper *p1, const __FlashStringHelper *p2, const __FlashStringHelper *p3 = nullptr) const;

	void logf(const char *p1, const char *format, ...) const __attribute__((format(printf, 3, 4)));
	void logf(const __FlashStringHelper *p1, const char *format, ...) const __attribute__((format(printf, 3, 4)));

	inline void log(const char *p1, const String &p2) const
	{
		log(p1, p2.c_str());
	}

	inline void log(const __FlashStringHelper *p1, const String &p2) const
	{
		log(p1, p2.c_str());
	}

	inline void log(const char *p1, const char *p2, const String &p3) const
	{
		log(p1, p2, p3.c_str());
	}

	inline void log(const __FlashStringHelper *p1, const char *p2, const String &p3) const
	{
		log(p1, p2, p3.c_str());
	}

	inline void log(const char *p1, const String &p2, const String &p3) const
	{
		log(p1, p2.c_str(), p3.c_str());
	}

	void writeEvent(String type, String src, String desc, String data);
	void writeLatest(String uid, String username, int acctype);

private:
	Stream *stream;
};

extern _Logger Logger;
extern HardwareSerial Serial;

#endif