

# 🎚FADER_X
<img align="right" src="https://user-images.githubusercontent.com/919746/193483114-41e6a3aa-9378-4de7-8e9b-f1b403b68153.png">

Teensy Firmware for FADER_4 and FADER_8 motorized fader banks from [Stage Hacks](https://www.stagehacks.com/store).

## Supports
- Generic MIDI
- QLab (MIDI + UDP OSC)
- ETC Eos (TCP OSC)
- X32/M32 (UDP OSC)
- DiGiCo (UDP OSC)


## Features
- Built-in web server hosts a configuration web app
- Up to 4 pages/banks of faders
- Advanced motion easing for motorized fader moves
- Custom OSC library

&nbsp;

&nbsp;

# Device Configuration
<img width="400px" src="https://user-images.githubusercontent.com/919746/193483214-978e690e-beae-4dc0-bc48-3eb1114815f1.png" align="right">

#### 🚨 Navigate to the IP Address of the FADER_X device in your browser to access the connfiguration web app. The default factory IP for the app is `192.168.1.130`. 🚨

## Generic MIDI
No OSC messages are sent when the FADER_X device is in MIDI mode. The configuration web server is active, and the FADER_X responds to `/page` messages.

### Parameters
| Parameter | Factory Value |  |
| ---- | ---- | ---- |
| Send Channel | 1 |  |
| Listen Channel | 1 |  |

---

## DiGiCo
On the Console
- Navigate the main screen to `Setup` > `External Control`.
- Ensure `External Control` is set to `YES`
- Add a new External Device if necessary
- Ensure the `Enabled` column displays a checkmark for all devices you wish to use.
- DiGiCo consoles on software V685 only support one OSC device.

On FADER_X Configuration web app
- Configure FADER_X `OSC Destination Port` parameter to match a OSC Receive port on your console.
- Configure FADER_X `Receive Port` parameter to match a OSC Send port on your console.

---

## QLab
- Connects to the console via UDP OSC. 
- Configure `OSC Destination Port` parameter to match QLab. QLab's default listening port is `53000`.
- QLab always replies on port `53001`.

| Parameter | Factory Value |  |
| ---- | ---- | ---- |
| OSC Passcode | 5353 | OSC Passcodes are enabled by default in QLab 5! |
| Maximum Volume | 12db | Should match `Max Volume Limit` in `Workspace Settings` / `Audio` |
| Maximum Volume | -60db | Should match `Min Volume Limit` in `Workspace Settings` / `Audio` |
| MIDI | - | When Page 3 or 4 is active, FADER_X will behave as if it's in MIDI mode. Useful for controlling QLab Lighting |
| Auto MIDI Switch | - | Toggle between pages 1/2 or 3/4 depending on if the selected cue has a Levels tab |

---

## ETC Eos
- Connects to the Eos console with a TCP connection on port `3037`. Make sure "Third Party OSC" is enabled in the Shell.
- `OSC Destination Port` parameter is ignored.

| Parameter | Factory Value |  |
| ---- | ---- | ---- |
| OSC Bank ID | 1 | Eos requires all attached OSC Fader Banks to have a unique ID. Remote apps for tablets have ID 0. |
| Fader Count | 40 | The highest number of fader you wish to control |

--- 

## X32/M32
- Connects to the console via UDP OSC on port `10023`. 
- `OSC Destination Port` parameter is ignored.

---





&nbsp;

&nbsp;

# Factory Resetting
Holding down the buttons labeled `DHCP` or `Static` for 5 seconds will reset the FADER_X device to factory settings. Depending on which button is held, it will restore to `192.168.1.130` Static IP or DHCP networking.

&nbsp;

# Reprogramming a FADER_X Device
## From precompiled .hex file
1) Install [Teensy Loader](https://www.pjrc.com/teensy/loader.html)
2) Download the latest binary from [Releases](https://github.com/stagehacks/FADER_X/releases)
3) Open Teensy Loader
4) Press the indicated button on your Teensy
5) Click the `Open HEX File` icon in Teensy Loader and select the binary file
6) Click the `Program` icon in Teensy Loader
7) When complete, click the `Reboot` icon in Teensy Loader

## From Source Code
In broad strokes, here's how to reprogram the Teensy 4.1 inside a FADER_X using Teensyduino. You should be somewhat comfortable with Arduino programming before going this route. 
1) Install [Teensyduino](https://www.pjrc.com/teensy/td_download.html)
2) Download this repository and open it in Teensyduino
3) Naviage to `Tools` > `Manage Libraries` to open the Library Manager
4) Install the libraries `QNEthernet` and `Easing`
5) Connect the Teensy to the computer with a USB Cable
6) Naviage to `Tools` > `Board` > `Teensyduino` and select Teensy 4.1
7) Naviage to `Tools` > `Port` and select the Teensy
8) Naviage to `Tools` > `USB Type` and select MIDI

&nbsp;

# Hardware Tuning
The physical resistance to touch on a fader can be dgusted using the following procedure.
1) Remove the front cover of the FADER_X using a M2.5 hex tool
2) For each fader to adjust, slightly loosen the screws holding the motor to the metal channel.
3) Adjust the motor's position to suit, then tighten the screws.

Changing the tension on the faders may require adustment of the `Minimum Speed` parameter in the configuration web app.
