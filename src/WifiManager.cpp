#include "WifiManager.hpp"
#include "Logger.hpp"

WifiManager::WifiManager()
{
}

void WifiManager::setup()
{
	WiFi.persistent(false);

	this->softAPModeStationConnected = WiFi.onSoftAPModeStationConnected(std::bind(&WifiManager::onSoftAPModeStationConnected, this, std::placeholders::_1));
	this->softAPModeStationDisconnected = WiFi.onSoftAPModeStationDisconnected(std::bind(&WifiManager::onSoftAPModeStationDisconnected, this, std::placeholders::_1));
	this->stationModeConnected = WiFi.onStationModeConnected(std::bind(&WifiManager::onStationModeConnected, this, std::placeholders::_1));
	this->stationModeDisconnected = WiFi.onStationModeDisconnected(std::bind(&WifiManager::onStationModeDisconnected, this, std::placeholders::_1));
	this->stationModeGotIP = WiFi.onStationModeGotIP(std::bind(&WifiManager::onStationModeGotIP, this, std::placeholders::_1));

	this->reload();
}

void WifiManager::reload() {
	// WifiSettings wifiSettings = settingsManager->getSettings().wifi;
	// if (wifiSettings.mode == WifiMode_AP)
	// {
	// 	LOG__INFO("Running WiFi in soft-AP mode. SSID = " + wifiSettings.ssid);
	// 	LOG__DEBUG("\tPassword = " + wifiSettings.password);
	// 	WiFi.mode(WIFI_AP);
	// 	if (WiFi.softAP(wifiSettings.ssid, wifiSettings.password))
	// 	{
	// 		LOG__DEBUG(F("Running WiFi in soft-AP mode. [Done]"));
	// 	}
	// 	else
	// 	{
	// 		LOG__ERROR("Cannot starting WiFi in soft-AP mode.");
	// 	}
	// 	LOG__INFO("\tIP address:" + WiFi.softAPIP().toString());
	// }
	// else if (wifiSettings.mode == WifiMode_CLIENT)
	// {
	// 	LOG__INFO("Running WiFi in client mode. SSID = " + wifiSettings.ssid);
	// 	LOG__DEBUG("\tPassword = " + wifiSettings.password);
	// 	WiFi.begin(wifiSettings.ssid, wifiSettings.password);
	// }
}

void WifiManager::onSoftAPModeStationConnected(const WiFiEventSoftAPModeStationConnected &evt)
{
	LOG__INFO("WiFi client connected: " + macToString(evt.mac));
}

void WifiManager::onSoftAPModeStationDisconnected(const WiFiEventSoftAPModeStationDisconnected &evt)
{
	LOG__INFO("WiFi client disconnected: " + macToString(evt.mac));
}

void WifiManager::onStationModeConnected(const WiFiEventStationModeConnected &evt)
{
	LOG__INFO("WiFi station connected: " + evt.ssid);
}

void WifiManager::onStationModeDisconnected(const WiFiEventStationModeDisconnected &evt)
{
	LOG__INFO("WiFi station disconnected: " + evt.ssid);
}

void WifiManager::onStationModeGotIP(const WiFiEventStationModeGotIP &evt)
{
	LOG__INFO("WiFi station got ip: " + evt.ip.toString());
	LOG__INFO("\tNetmask: " + evt.mask.toString());
	LOG__INFO("\tGateway: " + evt.gw.toString());
}

String WifiManager::macToString(const unsigned char *mac)
{
	char buf[20];
	snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return String(buf);
}