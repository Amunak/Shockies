#include "Remote.h"
#include "EepromSettings.h"

using namespace websockets;

void Remote::setup(EEPROM_Settings *settings) {
	if (this->client != nullptr) {
		this->client->close();
		delete this->client;
		this->client = nullptr;
		this->triedToConnect = false;

		Serial.println("Remote Access closed");
	}

	if (!settings->AllowRemoteAccess) {
		Serial.println("Remote Access disabled");
		return;
	}

	Serial.println(String("Starting Remote Access using \"") + settings->RemoteAccessEndpoint + "\"");

	this->triedToConnect = true;
	this->client = new WebsocketsClient();
	this->client->onMessage(std::bind(&Remote::onMessage, this, std::placeholders::_1));
	this->client->onEvent(std::bind(&Remote::onEvent, this, std::placeholders::_2));
	this->client->setInsecure();
	this->client->connect(settings->RemoteAccessEndpoint);

	if (!this->client->available()) {
		Serial.println("Failed to connect to remote control server");
		delete this->client;
		this->client = nullptr;
		return;
	}
}

State Remote::status() {
	if (this->client == nullptr) {
		return this->triedToConnect ? State::ConnectionFailed : State::Disabled;
	}

	return this->client->available(true) ? State::Connected : State::Disconnected;
}

void Remote::poll() {
	if (this->client != nullptr) {
		this->client->poll();
	}
}

void Remote::send(const char* data) {
	if (this->client != nullptr) {
		Serial.println(String("[Remote] Sending: ") + data);

		this->client->send(data);
	}
}

void Remote::onMessage(const WebsocketsMessage& message) {
	Serial.print("[Remote] Got Message: ");

	if (message.isBinary()) {
		Serial.println("Binary messages are not supported.");
		return;
	}

	if (!message.isComplete()) {
		Serial.println("Partial messages are not supported.");
		return;
	}

	Serial.println(message.data());

	this->commandHandler(message.c_str());
}

void Remote::onEvent(WebsocketsEvent event) {
	switch (event) {
		case WebsocketsEvent::ConnectionOpened:
			Serial.println("[Remote] Connected");
			this->connectHandler();
			break;
		case WebsocketsEvent::ConnectionClosed:
			Serial.println("[Remote] Disconnected");
			this->disconnectHandler();
			break;
		case WebsocketsEvent::GotPing:
			Serial.println("[Remote] Got a Ping!");
			break;
		case WebsocketsEvent::GotPong:
			Serial.println("[Remote] Got a Pong!");
			break;
		default:
			Serial.println("[Remote] Unknown event");
			break;
	}
}
