#ifndef WIFIMANAGER_HPP
#define WIFIMANAGER_HPP

#include <ESP8266WiFi.h>

class WifiManager {
public:
	WifiManager();
	void setup();
	void reload();

private:
	WiFiEventHandler softAPModeStationConnected;
	WiFiEventHandler softAPModeStationDisconnected;
	WiFiEventHandler stationModeConnected;
	WiFiEventHandler stationModeDisconnected;
	WiFiEventHandler stationModeGotIP;

	void onSoftAPModeStationConnected(const WiFiEventSoftAPModeStationConnected &evt);
	void onSoftAPModeStationDisconnected(const WiFiEventSoftAPModeStationDisconnected &evt);
	void onStationModeConnected(const WiFiEventStationModeConnected &evt);
	void onStationModeDisconnected(const WiFiEventStationModeDisconnected &evt);
	void onStationModeGotIP(const WiFiEventStationModeGotIP &evt);
	String macToString(const unsigned char *mac);
};

#endif