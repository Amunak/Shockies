#ifndef _remote_h
#define _remote_h

#include "EepromSettings.h"
#include <HTTPClient.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

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
	void poll();
	void send(const char *data);
private:
	std::function<void(const char *data)> commandHandler;
	std::function<void()> connectHandler;
	std::function<void()> disconnectHandler;
	WebsocketsClient *client = nullptr;
	void onEvent(WebsocketsEvent event);
	void onMessage(const WebsocketsMessage &message);
	bool triedToConnect = false;
};

#endif
