#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <ESPAsyncWebServer.h>
#include "Logger.hpp"

class AsyncLoggerWebHandler : public AsyncWebHandler
{
public:
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	virtual void handleRequest(AsyncWebServerRequest *request) override final{};
	virtual void handleUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) override final{};
	virtual void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) override final{};
	virtual bool isRequestHandlerTrivial() override final { return true; }
};

class _HttpServer : public AsyncWebServer
{
public:
	_HttpServer();
	void setup();
	void getApiIndex(AsyncWebServerRequest *request);

private:
	AsyncLoggerWebHandler loggerHandler;

	bool sendError(AsyncWebServerRequest *request, const String &code, const String &input = "");
	void onNotFoundRequest(AsyncWebServerRequest *request);
};

extern _HttpServer HttpServer;

#endif
