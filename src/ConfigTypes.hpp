#ifndef CONFIGTYPES_HPP
#define CONFIGTYPES_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ntp.h>

#define WifiMode_CLIENT 0
#define WifiMode_AP 1

struct NetworkConfig
{
	int mode;
	String ssid;
	String password;
	String apip;
	String apsubnet;
	String dns;
	bool hide;
	bool dhcp;
	String ip;
	String subnet;
	String gateway;
	String bssid;
	int failedConnectCountFallback;
	inline void fromJsonDocument(JsonDocument *doc, const char* key)
	{
		bssid = (*doc)[key]["bssid"].as<String>();
		ssid = (*doc)[key]["ssid"].as<String>();
		mode = (*doc)[key]["wmode"].as<int>();
		hide = (*doc)[key]["hide"].as<bool>();
		password = (*doc)[key]["pswd"].as<String>();
		dhcp = (*doc)[key]["dhcp"].as<bool>();
		ip = (*doc)[key]["ip"].as<String>();
		subnet = (*doc)[key]["subnet"].as<String>();
		gateway = (*doc)[key]["gateway"].as<String>();
		dns = (*doc)[key]["dns"].as<String>();
		apip = (*doc)[key]["apip"].as<String>();
		apsubnet = (*doc)[key]["apsubnet"].as<String>();
		failedConnectCountFallback = (*doc)[key].containsKey("failed_connect_count") ?
			(*doc)[key]["failed_connect_count"].as<int>() :
			50;
	}
	inline void addToJsonDocument(JsonDocument *doc, const char* key)
	{
		(*doc)[key] = (*doc).add<JsonObject>();
		(*doc)[key]["wmode"] = mode;
		(*doc)[key]["ssid"] = ssid;
		(*doc)[key]["pswd"] = password;
		(*doc)[key]["apip"] = apip;
		(*doc)[key]["apsubnet"] = apsubnet;
		(*doc)[key]["dns"] = dns;
		(*doc)[key]["hide"] = hide;
		(*doc)[key]["dhcp"] = dhcp;
		(*doc)[key]["ip"] = ip;
		(*doc)[key]["subnet"] = subnet;
		(*doc)[key]["gateway"] = gateway;
		(*doc)[key]["bssid"] = bssid;
		(*doc)[key]["failed_connect_count"] = failedConnectCountFallback;
	}
};

struct HardwareConfig
{
	unsigned int rtime;
	inline void fromJsonDocument(JsonDocument *doc, const char* key)
	{
		rtime = (*doc)[key]["rtime"].as<unsigned int>();
	}
	inline void addToJsonDocument(JsonDocument *doc, const char* key)
	{
		(*doc)[key] = (*doc).add<JsonObject>();
		(*doc)[key]["rtime"] = rtime;
	}
};

struct GeneralConfig
{
	String password;
	String hostname;
	unsigned int restart;
	inline void fromJsonDocument(JsonDocument *doc, const char* key)
	{
		hostname = (*doc)[key]["hostnm"].as<String>();
		restart = (*doc)[key]["restart"].as<unsigned int>();
		password = (*doc)[key]["pswd"].as<String>();

	}
	inline void addToJsonDocument(JsonDocument *doc, const char* key)
	{
		(*doc)[key] = (*doc).add<JsonObject>();
		(*doc)[key]["paswd"] = password;
		(*doc)[key]["restart"] = restart;
		(*doc)[key]["hostnm"] = hostname;

	}
};

struct MqttConfig
{
	bool enabled;
	String host;
	int port;
	String topic;
	String user;
	String password;
	inline void fromJsonDocument(JsonDocument *doc, const char* key)
	{
		enabled = (*doc)[key]["enabled"].as<bool>();
		host = (*doc)[key]["host"].as<String>();
		port = (*doc)[key]["port"].as<int>();
		topic = (*doc)[key]["topic"].as<String>();
		user = (*doc)[key]["user"].as<String>();
		password = (*doc)[key]["pswd"].as<String>();

	}
	inline void addToJsonDocument(JsonDocument *doc, const char* key)
	{
		(*doc)[key] = (*doc).add<JsonObject>();
		(*doc)[key]["enabled"] = enabled;
		(*doc)[key]["host"] = host;
		(*doc)[key]["port"] = port;
		(*doc)[key]["topic"] = topic;
		(*doc)[key]["user"] = user;
		(*doc)[key]["pswd"] = password;
	}
};

struct NtpConfig
{
	String server;
	int interval;
	int timezone;
	inline void fromJsonDocument(JsonDocument *doc, const char* key)
	{
		server = (*doc)[key]["server"].as<String>();
		interval = (*doc)[key]["interval"].as<unsigned int>();
		timezone = (*doc)[key]["timezone"].as<int>();
	}
	inline void addToJsonDocument(JsonDocument *doc, const char* key)
	{
		(*doc)[key] = (*doc).add<JsonObject>();
		(*doc)[key]["server"] = server;
		(*doc)[key]["interval"] = interval;
		(*doc)[key]["timezone"] = timezone;
	}
};

#endif