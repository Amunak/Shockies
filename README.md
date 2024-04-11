# Shockies
ESP32 Firmware for controlling **Petrainer PET998DR** and **FunniPets** collars using a **STX882 433.9Mhz ASK Transmitter**

## Disclaimer
I am not responsible for how you use or misuse this software, or any hardware controlled by this software.

I am not responsible for any malfunctions, and make no guarantees about safety or suitability of this software for any purpose beyond proof of concept.

**Shock collars are NOT designed for human use.**

**NEVER set the shock intensity above 50%**

**NEVER use a shock collar if you have a history of heart conditions, or are using a pacemaker**

**NEVER use a shock collar on your neck or chest area**

## Hardware
### Required Items
* Petrainer PET998DR or FunniPets collars
  - PET998DR can be found on [eBay](https://www.ebay.com/itm/181705501723)
  - FunniPets collars can be found on [Amazon](https://www.amazon.com/FunniPets-Training-Collar-Accessory-Receiver/dp/B0874GMM93/)
* ESP32 (NOT ESP32-S2!)
  - [ESP32 Developer boards](https://www.amazon.com/dp/B0718T232Z) are easiest to work with
  - [ESP32-WROOM-32D](https://www.digikey.com/en/products/detail/espressif-systems/ESP32-WROOM-32D-4MB/9381716) will also work
  - Other modules may work, but be careful
* STX882 ASK Transmitter for 433.9Mhz
  - Can be found on [Amazon](https://www.amazon.com/dp/B09KY28VH8)
* MicroUSB Cable
  - Used to connect to your PC, or to your power supply in standalone mode.
* [Male to Female Jumpers](https://www.amazon.com/HiLetgo-Breadboard-Prototype-Assortment-Raspberry/dp/B077X7MKHN)
* Soldering iron + Solder
  - Used to connect the jumpers and antenna to the STX882 Transmitter card.
## Assembly
You will need the following:
 * ESP32
 * STX882 Transmitter
 * STX882 Transmitter Antenna (looks like a coiled spring)
 * Male to Female Jumper Wire
 * Soldering Iron
 * Solder

1. Preheat Soldering iron to the temperature required for the solder you're using.
2. Separate a set of 3 jumper wires from the bundle, and set the remaining wires aside. You only need 3 Male to Female wires for this project.
3. Carefully solder the antenna to the ANT connection on the STX882 Transmitter. It should point straight up, away from the body of the transmitter for best range.
   - The ANT pin might be unlabled - it is the hole at the top of the card, opposite the DATA/VCC/GND Pins.
4. Carefully solder the Male side of each jumper to the DATA, VCC, and GND pins on the STX882 Transmitter.
   - Color doesn't really matter here - use whatever you'd like, just make sure they're visualy distinct from each other.
6. Turn off soldering iron, it is no longer needed.

Once the transmitter is sufficiently cool, make the following connections:
| ESP32 | Transmitter|
| ----- | ---------- |
| +5V   | VCC        |
| GND   | GND        |
| P4    | DATA       |

Once you verify these connections are correct, you're now ready to connect the ESP32 to your computer, and move on to **Setup**

## Setup
Setting up Shockies and your collar
### Visual Studio Code + PlatformIO & Flashing
1. Download the VSCode IDE from the [visual studio code website](https://code.visualstudio.com/download)
2. Install the PlatformIO IDE extension in VSCode
3. Open the Shockies project folder in VSCode, and allow PlatformIO time to get setup
4. Select the PlatformIO tab on the left side of the VSCode window, and select a task (such as *Upload** or *Upload and Monitor*) to build and upload the shockies code for your device. If you are using a different ESP32 board than the default, select *PIO Home > Boards* and select a different board configuration.

### Wi-Fi Setup
During the first boot, the Serial Monitor will display something similar to the following:
```
Device is booting...
Failed to connect to Wi-Fi
Creating temporary Access Point for configuration...
Access Point created!
SSID: ShockiesConfig
Pass: zappyzap
Starting mDNS...
Starting HTTP Server on port 80...
Starting WebSocket Server on port 81...
Connect to one of the following to configure settings:
http://shockies.local
http://192.168.4.1
```
1. Connect to the `ShockiesConfig` Wi-Fi Network, with the password `zappyzap`
   - Your device may complain about lack of internet - ignore it for now.
2. Navigate to http://shockies.local or http://192.168.4.1 - this will bring you to the Wi-Fi network configuration page.
   - If this doesn't work, verify that your device hasn't automatically disconnected from the `ShockiesConfig` Network
3. Input the SSID and Password of the network you want your ESP32 to connect to
4. Press **Save**, the ESP32 will now Reboot.
 
The Serial Monitor should now display something similar to the following:
```
Wi-Fi configuration changed, rebooting...
Device is booting...
Connecting to Wi-Fi...
SSID: mySSID
Wi-Fi Connected!
Starting mDNS...
Starting HTTP Server on port 80...
Starting WebSocket Server on port 81...
Connect to one of the following to configure settings:
http://shockies.local
http://192.168.2.2 
```
You can now access your device using http://shockies.local or using the IP address shown in the Serial Monitor.

### Configuration
The configuration page found on http://shockies.local allows you to set maximum values for intensity and duration for each of the 3 mappable collars.

#### Device Settings
- **Device ID**: A unique identifier for the "controller"/transmitter. This is used to differentiate between different controllers, and to prevent one controller from interfering with another. The collars pair to this ID.
- **Device Type**: The type of collar you are using. Currently you can choose between the *PET998DR Clone* and *FunniPets / Aliexpress Generic* collars.
- **Keep-Alive**: If enabled, the device will periodically make a "keep-alive" transmission (of a short vibration at 0 intensity) to the collar to prevent it from going to sleep.

#### Enabled Features
Allows the user to specify which collar features are enabled.
* Light
* Beep
* Vibrate
* Shock
Not all features will be available on every collar, so these settings may not have any noticable effect.

#### Maximum Intensity Settings
Specifies the maximum allowable intensity and continuious duration for Shock and Vibrate commands.
* Max Shock Intensity - Default 30%
* Max Shock Duration - Default 5 Seconds
* Shock Interval - Default 3 Seconds
* Max Vibrate Intensity - Default 100%
* Max Vibrate Duration - Default 5 Seconds

Please **CAREFULLY** experiment starting with low (<5%) shock intensity, and slowly increase it until you find a suitable maximum intensity.
The **Shock Interval** setting 

#### Security Settings
- **Require Device ID for local control**: If enabled, the device will require the use of a generated UUID for the websocket connection to control the device. You can see the UUID at the bottom of the page when you enable this option.
- **Command Access Key**: Commands sent to the device must include this key as the last parameter in the command. This is to prevent unauthorized access to the device.
- **Allow Remote Access**: Not yet implemented


## Firmware Updates
The updates page on http://shockies.local/update allows you to remotely update the firmware on your device. The username will be `admin`, and the password will be the same as your WiFi network password.
From this page, you can upload `firmware.bin` and `spiffs.bin` which can be found in the [releases](https://github.com/Aerizeon/Shockies/releases) section.


## Shockies Websocket Protocol
The Shockies Websocket Protocol is a very simple text-based protocol. Each message starts with a command, followed by a space, and then any parameters for that command. Each parameter is separated by a space.

When the connection is first established, the device will respond with a `OK: CONNECTED` message.
Shockies will also send current configuration to the client.

### Config Message Format
The config message is sent in the following format: `CONFIG:<hex data>`. It represents the current configuration of the device.
You will receive it when you connect to the device, and any time the configuration is changed - you need to be prepared to handle this message at any time.

Note that currently only the configuration for the first device is sent.

The hex data is formatted as follows:
```
CONFIG:0000XXYYZZAABB
```

- `0000`: A placeholder for future use.
- `XX`: Describes enabled features. Each bit represents a feature; if the bit is set, the feature is enabled.
  - `0x01`: Shock
  - `0x02`: Vibrate
  - `0x04`: Beep
  - `0x08`: Light
- `YY`: Maximum shock intensity setting for the device.
- `ZZ`: Maximum shock duration setting for the device.
- `AA`: Maximum vibrate intensity setting for the device.
- `BB`: Maximum vibrate duration setting for the device.

For example, a message might look like this: `CONFIG:00000F1E056405`.
This would represent a device with all (current) features enabled, a maximum shock intensity of 30%, a maximum shock duration of 5 seconds,
a maximum vibrate intensity of 100%, and a maximum vibrate duration of 5 seconds.

### Commands
Note that whenever you send an "action" command, it will keep sending (transmitting) until you send a `R` command to reset it
or until the maximum duration is reached - the duration itself is not a part of any command.

You should wait at least about ~300ms between resetting a command if you want to be sure it has triggered, as the collar may not respond immediately.

The `channel` parameter is a number from 0 to 2, and the `intensity` parameter is a number from 0 to 100 (or less, depending on the maximum intensity setting).

The `access key` parameter is a string that you set in the configuration page, and is used to prevent unauthorized access to the device.
If you do not set an access key, you can omit it from the command as it is ignored.

| Command                                | Example             | Response                | Description                                                                                                                                                                             |
|----------------------------------------|---------------------|-------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `P`                                    | `P`                 | -                       | Pings the device to keep the Websocket connection alive.                                                                                                                                |
| `X`                                    | `X`                 | `OK: EMERGENCY STOP`    | Trigger an emergency stop on the device. It will need to be reset manually.                                                                                                             |
| `R`                                    | `R`                 | `OK: R`                 | Resets the current command and stops transmitting.                                                                                                                                      |
| `L <channel> <intensity> <access key>` | `L 2 100`           | `OK: L`                 | Triggers the light on the collar. Note that `intensity` likely has no effect here. (The example would trigger the light for the third device.)                                          |
| `B <channel> <intensity> <access key>` | `B 1 1 verysecret`  | `OK: B`                 | Triggers the beep on the collar. Note that `intensity` likely has no effect here. (The example would trigger the beep for the second device if you set the access key to `verysecret`.) |
| `V <channel> <intensity> <access key>` | `V 42 100`          | `OK: V`                 | Starts the vibration on the collar. (The example would start the vibration for the first device at 42% intensity, and only if there was no access key required.)                        |
| `S <channel> <intensity> <access key>` | `S 0 30 verysecret` | `OK: S`                 | Starts the shock on the collar. (The example would start the shock for the first device at 30% intensity, assuming the access key was set to `verysecret`.)                             |

#### Other Responses
| Response                    | Description                                                           |
|-----------------------------|-----------------------------------------------------------------------|
| `ERROR: EMERGENCY STOP`     | The device is in an emergency stop state. You must reset it manually. |
| `ERROR: INVALID FORMAT`     | The command sent was invalid (most likely missing some arguments)     |
| `ERROR: INVALID ACCESS KEY` | The access key is required and either was not sent or is incorrect.   |
| `ERROR: INVALID ID`         | The device ID is out of range.                                        |

Note that if the device does not respond to a command you are probably sending one that is disabled.
It will also not respond if you send a non-existent command.

## Resonite Control
Message Epsilion for more information.

An alpha implementation can be found in the follwing public folder:

`resrec:///U-Epsilion/R-4af8f73a-3765-4ff7-ada0-cdd199286215`

## Web Control
*Not yet implemented*
