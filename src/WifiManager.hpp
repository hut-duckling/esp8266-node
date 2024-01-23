#ifndef WIFIMANAGER_HPP
#define WIFIMANAGER_HPP

#include <ESP8266WiFi.h>
#include <ConfigTypes.hpp>

class _WifiManager {
public:
	_WifiManager();
	void setup();
	void reload(NetworkConfig *network);
	void fallbackToApModeWithDefaultCredential();
	IPAddress getIpAddress();
	String getWifiSSID();
	String getDefaultHostname();
private:
	WiFiEventHandler softAPModeStationConnected;
	WiFiEventHandler softAPModeStationDisconnected;
	WiFiEventHandler stationModeConnected;
	WiFiEventHandler stationModeDisconnected;
	WiFiEventHandler stationModeGotIP;
	unsigned int mdnsInterval = 0;

	void setupMDNS();

	void onSoftAPModeStationConnected(const WiFiEventSoftAPModeStationConnected &evt);
	void onSoftAPModeStationDisconnected(const WiFiEventSoftAPModeStationDisconnected &evt);
	void onStationModeConnected(const WiFiEventStationModeConnected &evt);
	void onStationModeDisconnected(const WiFiEventStationModeDisconnected &evt);
	void onStationModeGotIP(const WiFiEventStationModeGotIP &evt);
	String macToString(const unsigned char *mac);
};

extern _WifiManager WifiManager;

#endif