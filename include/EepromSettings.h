#ifndef _eepromsettings_h
#define _eepromsettings_h

#include <DNSServer.h>
#include <mutex>
#include <Transmit.h>
#include <ESPAsyncWebServer.h>
#include "Devices.h"

#define SSID_LEN 33
#define WIFI_PASSWORD_LEN 65
#define UUID_STR_LEN 37
#define REMOTE_ENDPOINT_LEN 256
#define REMOTE_ID_LEN 129
#define ACCESS_KEY_LEN 65

/**
 * Stored EEPROM Settings
 */
struct EEPROM_Settings
{
	uint16_t SettingsVersion;
	/// Name of the Wi-Fi SSID to connect to on boot
	char WifiName[SSID_LEN];
	/// Password for the Wi-Fi network
	char WifiPassword[WIFI_PASSWORD_LEN];
	/// Device UUID for websocket endpoint.
	char DeviceId[UUID_STR_LEN];
	/// Require DeviceID to be part of the local websocket URI (ws://shockies.local/<deviceID>)
	bool RequireDeviceId;
	/// Allow the device to be controlled from shockies.dev. This only works for me at the moment.
	bool AllowRemoteAccess;
	/// Remote control endpoint (i.e. wss://shockies.dev/ws)
	char RemoteAccessEndpoint[REMOTE_ENDPOINT_LEN];
	/// Additional key required with each command
	char CommandAccessKey[ACCESS_KEY_LEN];
	/// Allow up to 3 devices to be configured
	Settings Devices[3];
};
#endif
