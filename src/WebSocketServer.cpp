#include "WebSocketServer.hpp"
#include <ArduinoJson.h>
#include <Logger.hpp>
#include "WebSocketServerResponse.esp"


_WebSocketServer WebSocketServer;

_WebSocketServer::_WebSocketServer() : AsyncWebSocket("/ws")
{

}

void _WebSocketServer::setup()
{
	this->onEvent(std::bind(&_WebSocketServer::onWsEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
}

void _WebSocketServer::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
	if (type == WS_EVT_ERROR) {
		LOG__WARN_F("WebSocket[%s][%u] error(%u): %s", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
	}
	else if (type == WS_EVT_CONNECT)
	{
		LOG__INFO("[" + client->client()->remoteIP().toString() + "] " + server->url() + " WebSocket Client connected");
	}
	else if (type == WS_EVT_DISCONNECT)
	{
		LOG__INFO(F("WebSocket Client disconnected"));
	}
	else if (type == WS_EVT_DATA)
	{
		AwsFrameInfo *info = (AwsFrameInfo *)arg;
		uint64_t index = info->index;
		uint64_t infolen = info->len;
		if (info->final && info->index == 0 && infolen == len)
		{
			// the whole message is in a single frame and we got all of it's data
			client->_tempObject = malloc(len);
			if (client->_tempObject != NULL)
			{
				memcpy((uint8_t *)(client->_tempObject), data, len);
			}
			this->process(client, infolen);
		}
		else
		{
			if (index == 0)
			{ // message is comprised of multiple frames or the frame is split into multiple packets
				if (info->num == 0 && client->_tempObject == NULL)
				{
					client->_tempObject = malloc(infolen);
				}
			}
			if (client->_tempObject != NULL)
			{
				memcpy((uint8_t *)(client->_tempObject) + index, data, len);
			}
			if ((index + len) == infolen)
			{
				if (info->final)
				{
					this->process(client, infolen);
				}
			}
		}
	}
}

void ICACHE_FLASH_ATTR _WebSocketServer::process(AsyncWebSocketClient *client, size_t size)
{
	JsonDocument root; // We should always get a JSON object (stringfied) from browser, so parse it

	char json[size + 1];
	memcpy(json, (char *)(client->_tempObject), size);
	json[size] = '\0';
	DeserializationError error = deserializeJson(root, json);
	if (error)
	{
		LOG__WARN(F("Couldn't parse WebSocket message"));
		free(client->_tempObject);
		client->_tempObject = NULL;
		// delete client->_tempObject;
		return;
	}
	if (!root.containsKey("command")) {
		free(client->_tempObject);
		client->_tempObject = NULL;
	}

	const char *command = root["command"];
	LOG__INFO_F("Websocket request, command: %s", command);

	if (strcmp(command, "status") == 0) {
		wsSendStatus();
	}
	else if (strcmp(command, "getconf") == 0) {
		wsSendConfig();
	}
	else if (strcmp(command, "configfile") == 0) {
		wsWriteConfigFileFromJsonDocument(&root);
	}
	else if (strcmp(command, "gettime") == 0) {
		wsSendTime();
	}
	else if (strcmp(command, "settime") == 0) {
		if (root.containsKey("epoch")) {
			time_t time = root["epoch"].as<time_t>();;
			setTime(time);
		}
	}
	else if (strcmp(command, "scan") == 0) {
		WiFi.scanNetworksAsync(wsSendWifiScanResult, true);
	}
	else if (strcmp(command, "userlist") == 0) {
		int page = root["page"];
		wsSendUserList(page, client);
	}
	else if (strcmp(command, "remove") == 0) {
		const char *uid = root["uid"];
		String filename = "/P/";
		filename += uid;
		LOG__INFO_F("Try removing: %s", filename.c_str());
		bool result = LittleFS.remove(filename);
		if (result) {
			LOG__INFO_F("Try removing: %s [done]", filename.c_str());
		} else {
			LOG__INFO_F("Try removing: %s [faild]", filename.c_str());

			LOG__INFO_F("Try removing: %s using SPIFFS", filename.c_str());
			if (SPIFFS.remove(filename.c_str())) {
				LOG__INFO_F("Try removing: %s using SPIFFS [done]", filename.c_str());
			} else {
				LOG__INFO_F("Try removing: %s using SPIFFS [faild]", filename.c_str());

				LittleFS.rmdir(filename.c_str());
			}
		}
		wsSendUserList(1, client);
	}
	else if (strcmp(command, "geteventlog") == 0)
	{
		int page = root["page"];
		sendEventLog(page);
	}
	else if (strcmp(command, "getlatestlog") == 0)
	{
		int page = root["page"];
		sendLatestLog(page);
	}
	else if (strcmp(command, "clearevent") == 0)
	{
		LittleFS.remove("/eventlog.json");
		Logger.writeEvent("WARN", "sys", "events cleared!", "");
	}
	else if (strcmp(command, "clearlatest") == 0)
	{
		LittleFS.remove("/latestlog.json");
		Logger.writeEvent("WARN", "sys", "Logs cleared!", "");
	}
	else if (strcmp(command, "writeevent") == 0) {
		// const char *uid = root["uid"];
		// writeEvent();
	}
	else if (strcmp(command, "userfile") == 0) {
		LOG__INFO("got new user to store");
		const char *uid = root["uid"];
		String filename = "/P/";
		filename += uid;
		LOG__DEBUG_F("Open file: %s to store user file", filename.c_str());
		File file = LittleFS.open(filename, "w+");
		// Check if we created the file
		if (file)
		{
			LOG__DEBUG_F("Open file: %s to store user file [done]", filename.c_str());

			String data;
			serializeJson(root, data);
			LOG__DEBUG_F("Store user data in file: %s data: %s", filename.c_str(), data.c_str());

			auto writesize = serializeJson(root, file);
			// size_t writesize = file.write(data.c_str());
			LOG__DEBUG_F("Store %u user data in file: %s [done]", writesize, filename.c_str());
		}
		file.close();
		WebSocketServer.textAll("{\"command\":\"result\",\"resultof\":\"userfile\",\"result\": true}");
	}

	free(client->_tempObject);
	client->_tempObject = NULL;
}