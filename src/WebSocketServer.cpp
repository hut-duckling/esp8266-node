#include "WebSocketServer.hpp"
#include <ArduinoJson.h>
#include <Logger.hpp>

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
	// We should always get a JSON object (stringfied) from browser, so parse it
	JsonDocument root;

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
	const char *command = root["command"];

	free(client->_tempObject);
	client->_tempObject = NULL;
}