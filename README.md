# FADER_X
 
Firmware for FADER_4 and FADER_8 motorized fader banks from [Stage Hacks](https://www.stagehacks.com/store).

## Supports
- Generic MIDI
- QLab (MIDI + OSC)
- ETC Eos (OSC)
- X32/M32 (OSC)
- DiGiCo (OSC)


## Features
- Built-in web server hosts a configuration web app
- Up to 4 pages/banks of faders
- Advanced motion easing for motorized fader moves

&nbsp;

&nbsp;

# Factory Resetting
Holding down the buttons labeled `DHCP` or `Static` for 5 seconds will reset the FADER_X device to factory settings. Depending on which button is held, it will restore to `192.168.1.130` Static IP or DHCP networking.


# Device Configuration
## Generic MIDI
No OSC messages are sent when the FADER_X device is in MIDI mode. The configuration web server is active, and the FADER_X responds to `/page` messages.

### Parameters
| Parameter | Factory Value |  |
| ---- | ---- | ---- |
| Send Channel | 1 |  |
| Listen Channel | 1 |  |

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
