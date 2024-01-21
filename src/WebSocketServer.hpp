#ifndef WEBSOCKETSERVER_HPP
#define WEBSOCKETSERVER_HPP

#include <ESPAsyncWebServer.h>

class _WebSocketServer : public AsyncWebSocket
{
public:
	_WebSocketServer();
	void setup();
protected:
	void process(AsyncWebSocketClient *client, size_t size);
private:
	void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
};

extern _WebSocketServer WebSocketServer;

#endif