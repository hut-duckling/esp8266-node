#include "HttpServer.hpp"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <Logger.hpp>
#include <SensorsManager.hpp>
#include <ConfigManager.hpp>
#include <WebSocketServer.hpp>

// these are from vendors
#include "webh/glyphicons-halflings-regular.woff.gz.h"
#include "webh/required.css.gz.h"
#include "webh/required.js.gz.h"
// these are from us which can be updated and changed
#include "webh/esprfid.js.gz.h"
#include "webh/esprfid.htm.gz.h"
#include "webh/index.html.gz.h"

_HttpServer HttpServer;

_HttpServer::_HttpServer() : AsyncWebServer(80), loggerHandler(AsyncLoggerWebHandler())
{
}

void ICACHE_FLASH_ATTR _HttpServer::setup()
{
	DefaultHeaders &defaultHeaders = DefaultHeaders::Instance();
	defaultHeaders.addHeader("Access-Control-Allow-Origin", "*");
	defaultHeaders.addHeader("Access-Control-Allow-Credentials", "true");
	defaultHeaders.addHeader("Access-Control-Allow-Headers", "*");
	defaultHeaders.addHeader("Access-Control-Expose-Headers", "*");
	defaultHeaders.addHeader("Access-Control-Allow-Methods", "*");
	this->addHandler(&this->loggerHandler);

	this->addHandler(&WebSocketServer);

	this->on("/fonts/glyphicons-halflings-regular.woff", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "font/woff", glyphicons_halflings_regular_woff_gz, glyphicons_halflings_regular_woff_gz_len);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	});
	this->on("/css/required.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", required_css_gz, required_css_gz_len);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	});
	this->on("/js/required.js", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "text/javascript", required_js_gz, required_js_gz_len);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	});
	this->on("/js/esprfid.js", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "text/javascript", esprfid_js_gz, esprfid_js_gz_len);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	});
	this->on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html_gz, index_html_gz_len);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	});
	this->on("/esprfid.htm", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", esprfid_htm_gz, esprfid_htm_gz_len);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	});

	this->on("/login", HTTP_GET, [](AsyncWebServerRequest *request) {
		String remoteIP = request->client()->remoteIP().toString();
		
		// if (!request->authenticate(http_username, http_pass)) {
			// writeEvent("WARN", "websrv", "New login attempt", remoteIP);
			// return request->requestAuthentication();
		// }
		request->send(200, "text/plain", "Success");
		// writeEvent("INFO", "websrv", "Login success!", remoteIP);
	});
	this->rewrite("/index", "/index.html");
	this->rewrite("/", "/index.html");
	this->begin();

	this->on("/api/v1", HTTP_GET, std::bind(&_HttpServer::getApiIndex, this, std::placeholders::_1));
	this->onNotFound(std::bind(&_HttpServer::onNotFoundRequest, this, std::placeholders::_1));
}

void _HttpServer::getApiIndex(AsyncWebServerRequest *request)
{
	// StaticJsonDocument<512> doc;

	// doc["status"] = true;
	// JsonObject result = doc.createNestedObject("result");
	// result["product"] = doc.createNestedObject();
	// result["product"]["title"] = "HUT Duckling";
	// result["author"] = doc.createNestedObject();
	// result["author"]["name"] = "MohammadHasan Hosni";
	// result["author"]["website"] = "https://hut.ac.ir/";
	// result["owner"] = doc.createNestedObject();
	// result["owner"]["name"] = "Hamedan University Of Technology";
	// result["owner"]["website"] = "https://hut.ac.ir/";
	// result["sensors"] = doc.createNestedObject();
	// result["sensors"]["temperature"] = SensorsManager.getAvgTempreture();
	// result["sensors"]["light"] = SensorsManager.getLightPercent();

	// String output;
	// serializeJson(doc, output);
	// request->send(200, "application/json", output);
}

bool _HttpServer::sendError(AsyncWebServerRequest *request, const String &code, const String &input)
{
	// StaticJsonDocument<128> doc;

	// doc["status"] = false;
	// doc["error"] = code;
	// if (input.length() > 0)
	// {
	// 	doc["input"] = input;
	// }

	// String output;
	// serializeJson(doc, output);

	// request->send(400, "application/json", output);

	return false;
}

void _HttpServer::onNotFoundRequest(AsyncWebServerRequest *request) {
	if (request->method() == HTTP_OPTIONS) {
		request->send(200);
	} else {
		request->send(404, "text/html", String("<html>\
<head><title>404 Not Found</title></head>\
<body>\
<center><h1>404 Not Found</h1></center>\
<hr><center>HUT-Duckling/") + "</center>\
</body>\
</html>");
	}
}

bool AsyncLoggerWebHandler::canHandle(AsyncWebServerRequest *request)
{
	LOG__INFO(request->client()->remoteIP().toString() + " " + String(request->methodToString()) + " " + request->url());
	return false;
}
