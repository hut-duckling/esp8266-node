#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <Arduino.h>

#define WifiMode_AP 1
#define WifiMode_CLIENT 2

struct WifiSettings
{
	uint8_t mode;
	String ssid;
	String password;

	bool equals(const WifiSettings &other) const {
		return (this->mode == other.mode && this->ssid == other.ssid && this->password == other.password);
	};
};

class _ConfigManager {
public:
	void setup();
	void reset();
	void save();
};

extern _ConfigManager ConfigManager;

#endif