#ifndef _remote_h
#define _remote_h

#include "EepromSettings.h"
#include <HTTPClient.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

#define RECONNECT_INTERVAL 30000

enum class State {
	Disabled,
	Connected,
	Disconnected,
	ConnectionFailed,
};

class Remote {
public:
	Remote(void (*commandHandler) (const char *data), void (*connectHandler) (), void (*disconnectHandler) ()) {
		this->commandHandler = commandHandler;
		this->connectHandler = connectHandler;
		this->disconnectHandler = disconnectHandler;
	}

	/**
	 * (Re)Setup the connection to the remote control server,
	 * disabling it if configuration changed and it was enabled previously, or enabling it if it was disabled, or re-setting it if it was already enabled.
	 */
	void setup(EEPROM_Settings *settings);
	State status();
	/**
	 * Poll the remote control server for new commands. This should be called in the main loop.
	 * Does nothing if the remote control server is disabled.
	 * This function will also attempt to reconnect if the connection was lost or failed.
	 */
	void poll();
	void send(const String& data);
private:
	std::function<void(const char *data)> commandHandler;
	std::function<void()> connectHandler;
	std::function<void()> disconnectHandler;
	void onEvent(WebsocketsEvent event);
	void onMessage(const WebsocketsMessage &message);
	void connect();
	void disconnect();
	void tryReconnect();

	char remoteAccessEndpoint[REMOTE_ENDPOINT_LEN];
	WebsocketsClient *client = nullptr;
	bool triedToConnect = false;
	bool dispose = false;
	uint32_t lastConnectionAttempt = 0;

};

#endif
