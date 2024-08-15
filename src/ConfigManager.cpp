#include "ConfigManager.hpp"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <Logger.hpp>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <AsyncTimer.h>
#include <Ntp.h>
#include <ConfigTypes.hpp>
#include <WebSocketServer.hpp>
#include <WifiManager.hpp>

extern AsyncTimer asyncTimer;

_ConfigManager ConfigManager;

void _ConfigManager::load()
{
    File configFile = LittleFS.open("/config.json", "r");

    if (!configFile)
    {
        LOG__WARN("ConfigManager::load no such file!");
        return;
    }
    const String content = configFile.readString();
    LOG__INFO_F("ConfigManager::load file content: %s", content.c_str());

    JsonDocument config;
    DeserializationError error = deserializeJson(config, content);
    if (error)
    {
        LOG__WARN_F("ConfigManager::load failed to parse config file, code: %d error: %s", error.code(), error.c_str());
        return;
    }
    LOG__INFO_F("config size: %u is null? %s", config.size(), config.isNull() ? "YYY" : "NNN")

    if (config.containsKey("network"))
    {
        this->network.fromJsonDocument(&config, "network");
    }

    if (config.containsKey("hardware"))
    {
        this->hardware.fromJsonDocument(&config, "hardware");
    }

    if (config.containsKey("general"))
    {
        this->general.fromJsonDocument(&config, "general");
    }

    if (config.containsKey("mqtt"))
    {
        this->mqtt.fromJsonDocument(&config, "mqtt");
    }

    if (config.containsKey("ntp"))
    {
        this->ntp.fromJsonDocument(&config, "ntp");
    }
}

void _ConfigManager::setup()
{
    unsigned int failedCount = 0;
    wifiStatusMonitorIntervalId = asyncTimer.setInterval([&, failedCount]() mutable {
        LOG__TRACE("Check WiFi status");
        switch (WiFi.status())
        {
            case WL_NO_SHIELD:
                LOG__TRACE("Check WiFi status [WL_NO_SHIELD]");
                break;
            case WL_IDLE_STATUS:
                LOG__TRACE("Check WiFi status [WL_IDLE_STATUS]");
                break;
            case WL_NO_SSID_AVAIL:
                LOG__TRACE("Check WiFi status [WL_NO_SSID_AVAIL]");
                failedCount++;
                break;
            case WL_SCAN_COMPLETED:
                LOG__TRACE("Check WiFi status [WL_SCAN_COMPLETED]");
                break;
            case WL_CONNECTED:
                LOG__TRACE("Check WiFi status [WL_CONNECTED]");

                LOG__TRACE("Good time to setupNTP, try it");
                this->setupNTP();
                LOG__TRACE("Good time to setupNTP, try it [done]");
                break;
            case WL_CONNECT_FAILED:
                LOG__TRACE("Check WiFi status [WL_CONNECT_FAILED]");
                failedCount++;
                break;
            case WL_CONNECTION_LOST:
                LOG__TRACE("Check WiFi status [WL_CONNECTION_LOST]");
                break;
            case WL_WRONG_PASSWORD:
                LOG__WARN("Check WiFi status [WL_WRONG_PASSWORD]");
                WifiManager.fallbackToApModeWithDefaultCredential();
                break;
            case WL_DISCONNECTED:
                LOG__TRACE("Check WiFi status [WL_DISCONNECTED]");
                failedCount++;
                break;
        }
        if (network.failedConnectCountFallback && failedCount >= network.failedConnectCountFallback)
        {
            network.failedConnectCountFallback = 0;
            LOG__WARN("Number of faild connection for WiFi reached! fallback to AP");
            WifiManager.fallbackToApModeWithDefaultCredential();
            LOG__WARN("Number of faild connection for WiFi reached! fallback to AP [done]");
        }
    }, 3000);
}

void _ConfigManager::setupNTP()
{
    if (!this->ntpClient) {
        ntpClient = new NtpClient();
        const char *ntpserver = this->ntp.server.c_str();
        LOG__INFO_F("Set NTP server: %s", ntpserver);
        LOG__INFO_F("Set NTP timezone to: %d", this->ntp.timezone);
        ntpClient->Ntp(
            ntpserver,
            this->ntp.timezone,
            this->ntp.interval
        );
    }
}

void _ConfigManager::save()
{
}

void _ConfigManager::restart()
{
    LittleFS.end();
    WebSocketServer.enable(false);
    ESP.restart();
}
