#include <ShockiesRemote.h>
#include <AsyncTCP_SSL.h>

#include <utility>

AsyncSSLClient *client = nullptr;

ShockiesRemote::ShockiesRemote(const char *uuid)
{
	deviceUuid = uuid;
	client = new AsyncSSLClient();
}

void ShockiesRemote::connect(const char *addr, unsigned int port)
{
	this->addr = addr;
	this->port = port;

	client->connect(addr, port, true);
	client->onConnect(
		[](void *sr, AsyncSSLClient *c) {
			auto *remote = (ShockiesRemote *) sr;
			remote->connected(c);
		},
		this
	);
	client->onDisconnect(
		[](void *sr, AsyncSSLClient *c) {
			auto *remote = (ShockiesRemote *) sr;
			remote->disconnected(c);
		},
		this
	);
	client->onData(
		[](void *sr, AsyncSSLClient *c, void *d, size_t l) {
			auto *remote = (ShockiesRemote *) sr;
			remote->data(c, d, l);
		},
		this
	);
}

void ShockiesRemote::disconnect()
{
	_disconnecting = true;
	client->close();
}

void ShockiesRemote::sendCommand(const char *command)
{
	auto len = strlen(command);
	if (len + 1 < sizeof(dataBuf)) {
		strncpy(dataBuf + 1, command, len);
		dataBuf[0] = len;
		client->write(dataBuf, len + 1);
	}
}

bool ShockiesRemote::isConnected() const
{
	return _isConnected;
}

void ShockiesRemote::onCommand(CommandHandler handler)
{
	_commandHandler = std::move(handler);
}

void ShockiesRemote::onConnected(ConnectionHandler handler)
{
	_connectedHandler = std::move(handler);
}

void ShockiesRemote::onDisconnected(ConnectionHandler handler)
{
	_disconnectedHandler = std::move(handler);
}

void ShockiesRemote::connected(AsyncSSLClient *c)
{
	c->setNoDelay(true);
	sendCommand(deviceUuid);
	_isConnected = true;

	if (_connectedHandler != nullptr) {
		_connectedHandler();
	}
}

void ShockiesRemote::disconnected(AsyncSSLClient *client)
{
	_isConnected = false;
	if (_disconnectedHandler != nullptr) {
		_disconnectedHandler();
	}
	// This is probably a bad idea, but let's do it anyways
	// Try to reconnect if we disconnect, since this shouldn't normally happen.
	if (!_disconnecting) {
		client->connect(addr, port, true);
	}
	_disconnecting = false;
}

void ShockiesRemote::data(AsyncSSLClient *client, void *data, size_t len)
{
	char *str = (char *) data;

	if (str[0] == len - 1 && len > 0) {
		if (_commandHandler != nullptr) {
			_commandHandler(str + 1, len - 1);
		}
	} else {
		Serial.println(len);
	}
}
