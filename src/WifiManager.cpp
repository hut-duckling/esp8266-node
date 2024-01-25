#include "WifiManager.hpp"
#include "Logger.hpp"
#include "ConfigManager.hpp"
#include <ConfigTypes.hpp>
#include "ESP8266WiFiType.h"
#include "ESP8266mDNS.h"
#include <AsyncTimer.h>

extern AsyncTimer asyncTimer;
_WifiManager WifiManager;

_WifiManager::_WifiManager()
{
}

void _WifiManager::setup()
{
	WiFi.persistent(false);

	this->softAPModeStationConnected = WiFi.onSoftAPModeStationConnected(std::bind(&_WifiManager::onSoftAPModeStationConnected, this, std::placeholders::_1));
	this->softAPModeStationDisconnected = WiFi.onSoftAPModeStationDisconnected(std::bind(&_WifiManager::onSoftAPModeStationDisconnected, this, std::placeholders::_1));
	this->stationModeDisconnected = WiFi.onStationModeDisconnected(std::bind(&_WifiManager::onStationModeDisconnected, this, std::placeholders::_1));
	this->stationModeConnected = WiFi.onStationModeConnected(std::bind(&_WifiManager::onStationModeConnected, this, std::placeholders::_1));
	this->stationModeGotIP = WiFi.onStationModeGotIP(std::bind(&_WifiManager::onStationModeGotIP, this, std::placeholders::_1));

	this->reload(nullptr);
}

void _WifiManager::reload(NetworkConfig *network = nullptr) {
	if (network == nullptr) {
		network = &ConfigManager.network;
	}

	if (network->mode == WifiMode_AP)
	{
		LOG__INFO_F("Running WiFi in soft-AP mode. SSID = %s", network->ssid.c_str());
		LOG__DEBUG_F("\tPassword = %s", network->password.c_str());
		WiFi.mode(WIFI_AP);
		ConfigManager.network.failedConnectCountFallback = 0; // we already in AP mode

		IPAddress apip;
		apip.fromString(network->apip);
		LOG__DEBUG("\tAccessPoint-IP = " + apip.toString());

		IPAddress apsubnet;
		apsubnet.fromString(network->apsubnet);
		LOG__DEBUG("\tAccessPoint-Subnet = " + apsubnet.toString());

		String ssid = network->ssid.length() ?
			network->ssid :
			this->getDefaultHostname();

		WiFi.softAPConfig(apip, apip, apsubnet);
		if (WiFi.softAP(ssid.c_str(), network->password.c_str()))
		{
			LOG__DEBUG(F("Running WiFi in soft-AP mode. [Done]"));
		}
		else
		{
			LOG__ERROR("Cannot starting WiFi in soft-AP mode.");
		}
		LOG__INFO("\tIP address:" + WiFi.softAPIP().toString());
	}
	else if (network->mode == WifiMode_CLIENT)
	{
		LOG__INFO("Running WiFi in client mode. SSID = " + network->ssid);
		LOG__DEBUG("\tBSSID = " + network->bssid);
		LOG__DEBUG("\tPassword = " + network->password);
		WiFi.mode(WIFI_STA);

		if (network->dhcp == 0) {
			IPAddress clientip;
			IPAddress subnet;
			IPAddress gateway;
			IPAddress dns;
			clientip.fromString(network->ip);
			subnet.fromString(network->subnet);
			gateway.fromString(network->gateway);
			dns.fromString(network->dns);
			WiFi.config(clientip, gateway, subnet, dns);
		}
		if (network->dns.length()) {
			IPAddress dns;
			dns.fromString(network->dns);
			dns_setserver(0, dns);
		}
		WiFi.begin(network->ssid, network->password);
	}
	this->setupMDNS();
}
void _WifiManager::fallbackToApModeWithDefaultCredential()
{
	NetworkConfig temp={
		.mode=WifiMode_AP,
		.ssid=this->getDefaultHostname(),
		.password="12345678",
	};
	LOG__WARN_F("fallback To Ap Mode With Default Credential AP: %s password: %s", temp.ssid.c_str(), temp.password.c_str());
	this->reload(&temp);
}
IPAddress _WifiManager::getIpAddress()
{
	if (WiFi.getMode() == WiFiMode_t::WIFI_AP)
	{
		return WiFi.softAPIP();
	}
	else if (WiFi.getMode() == WiFiMode_t::WIFI_STA)
	{
		return WiFi.localIP();
	}
	IPAddress ip;
	ip.fromString("0.0.0.0");
	return ip;
}

String _WifiManager::getWifiSSID()
{
	if (WiFi.getMode() == WiFiMode_t::WIFI_AP)
	{
		return WiFi.softAPSSID();
	}
	else if (WiFi.getMode() == WiFiMode_t::WIFI_STA)
	{
		return ConfigManager.network.ssid;
	}
	return String("");
}

void _WifiManager::setupMDNS()
{
	String hostname = ConfigManager.general.hostname.length() ?
		ConfigManager.general.hostname :
		this->getDefaultHostname();
	hostname.toLowerCase();

	LOG__INFO_F("Set hostname to %s", hostname.c_str());
	WiFi.hostname(hostname.c_str());

	LOG__INFO_F("Setup MDNS to %s", hostname.c_str());
	if (MDNS.begin(hostname.c_str())) {
		LOG__INFO_F("Setup MDNS to %s [done]", hostname.c_str());
	} else {
		LOG__INFO_F("Setup MDNS to %s [failed]", hostname.c_str());
	}
	MDNS.addService("http", "tcp", 80);
	mdnsInterval = asyncTimer.setInterval([&]() {
		LOG__TRACE_F(
			"Update MDNS, isRunning? %s",
			MDNS.isRunning() ? "Yes" : "No"
		);
		MDNS.update();
	}, 1000);
}

void _WifiManager::onSoftAPModeStationConnected(const WiFiEventSoftAPModeStationConnected &evt)
{
	LOG__INFO("WiFi client connected: " + macToString(evt.mac));
}

void _WifiManager::onSoftAPModeStationDisconnected(const WiFiEventSoftAPModeStationDisconnected &evt)
{
	LOG__INFO("WiFi client disconnected: " + macToString(evt.mac));
}

void _WifiManager::onStationModeConnected(const WiFiEventStationModeConnected &evt)
{
	LOG__INFO("WiFi station connected: " + evt.ssid);
}

void _WifiManager::onStationModeDisconnected(const WiFiEventStationModeDisconnected &evt)
{
	LOG__INFO("WiFi station disconnected: " + evt.ssid);
	WiFi.reconnect();
}

void _WifiManager::onStationModeGotIP(const WiFiEventStationModeGotIP &evt)
{
	LOG__INFO("WiFi station got ip: " + evt.ip.toString());
	LOG__INFO("\tNetmask: " + evt.mask.toString());
	LOG__INFO("\tGateway: " + evt.gw.toString());
}

String _WifiManager::macToString(const unsigned char *mac)
{
	char buf[20];
	snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return String(buf);
}

String _WifiManager::getDefaultHostname()
{
	uint8_t macAddr[WL_MAC_ADDR_LENGTH];
	WiFi.softAPmacAddress(macAddr);
	char buf[24];
	sprintf(buf, "ESP-HUT-DUCKLING-%02x%02x%02x", macAddr[3], macAddr[4], macAddr[5]);
	return String(buf);
}
