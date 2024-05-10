#include "Remote.h"
#include "EepromSettings.h"

using namespace websockets;

void Remote::setup(EEPROM_Settings *settings) {
	if (client != nullptr) {
		disconnect();
		this->triedToConnect = false;

		Serial.println("Remote Access closed");
	}

	if (!settings->AllowRemoteAccess) {
		Serial.println("Remote Access disabled");
		return;
	}

	strcpy(this->remoteAccessEndpoint, settings->RemoteAccessEndpoint);

	connect();
}

void Remote::connect() {
	Serial.println(String("Starting Remote Access using \"") + this->remoteAccessEndpoint + "\"");

	triedToConnect = true;
	client = new WebsocketsClient();
	client->onMessage(std::bind(&Remote::onMessage, this, std::placeholders::_1));
	client->onEvent(std::bind(&Remote::onEvent, this, std::placeholders::_2));
	client->setInsecure();
	client->connect(this->remoteAccessEndpoint);
	lastConnectionAttempt = millis();

	if (!client->available()) {
		Serial.println("Failed to connect to remote control server");
		disconnect();
		return;
	}
}

void Remote::disconnect() {
	this->dispose = false;
	if (client == nullptr) {
		return;
	}

	client->close();
	delete client;
	client = nullptr;
	Serial.println("[Remote] Disposed");
}

void Remote::tryReconnect() {
	// if we have a client, we're connected
	if (client != nullptr) {
		return;
	}

	// if we haven't tried to connect yet, we are disabled
	if (!triedToConnect) {
		return;
	}

	// retry only every once in a while
	if (millis() - lastConnectionAttempt <= RECONNECT_INTERVAL) {
		return;
	}

	connect();
}

State Remote::status() {
	if (client == nullptr) {
		return triedToConnect ? State::ConnectionFailed : State::Disabled;
	}

	return client->available(true) ? State::Connected : State::Disconnected;
}

void Remote::poll() {
	if (dispose) {
		disconnect();
		return;
	}

	tryReconnect();

	if (client != nullptr) {
		client->poll();
	}
}

void Remote::send(const String& data) {
	if (client != nullptr) {
		Serial.println(String("[Remote] Sending: ") + data);

		client->send(data);
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

	commandHandler(message.c_str());
}

void Remote::onEvent(WebsocketsEvent event) {
	switch (event) {
		case WebsocketsEvent::ConnectionOpened:
			Serial.println("[Remote] Connected");
			connectHandler();
			break;
		case WebsocketsEvent::ConnectionClosed:
			dispose = true; // mark for disposal on next poll
			Serial.println("[Remote] Disconnected");
			disconnectHandler();
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
