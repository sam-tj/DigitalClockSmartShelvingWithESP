# DigitalClockSmartShelvingWithESP
DIY 3D printed clock working with NodeMCU (ESP-12)

The project uses NTP server to get the Time values and uses it to display it on the clock.

------------

### Libraries Required

You need to download the following:

- Adafruit NeoPixel Library: https://github.com/adafruit/Adafruit_NeoPixel
- NTPClient by taranais: https://github.com/taranais/NTPClient

------------

### List of components used:
- NodeMCU 1.0
- LDR Light Sensor [*for reference*](https://www.amazon.co.uk/Nitrip-Digital-Intensity-Detection-Photosensitive/dp/B07W9BQX64/ref=sr_1_3?keywords=Digital+Light+Intensity+Detection+Photosensitive&qid=1585488675&sr=8-3 "reference") 
- 5V Power Supply
- WS2812B LED Strip (60 leds / m) [*for reference*](https://www.amazon.co.uk/dp/B01CDTEJBG/ref=cm_sw_r_tw_dp_U_x_DLjGEbE6E76WZ "*for reference*")

------------

### Tutorial
This project is a fork from DIY Machines project "*Giant Hidden Shelf Edge Clock*"

Thus can use the same tutorial for assembly of the clock: https://www.instructables.com/id/How-to-Build-a-Giant-Hidden-Shelf-Edge-Clock/

3D Files can be found here: https://www.thingiverse.com/thing:4207524/files

------------

### Connections
| NodeMCU  | Clock LED  | Shelf LED  | LDR Sensor  |
| ------------ | ------------ | ------------ | ------------ |
| D6  | Data In  | -  | -  |
| D5  | -  | Data In  | -  |
| A0  | -  | -  | Analog Out  |
| Vin  | +5V  | +5V  | -  |
| 3.3V  | -  | -  | 3.3V  |
| GND  | GND  | GND  | GND  |

![Connections](https://raw.githubusercontent.com/sam-tj/DigitalClockSmartShelvingWithESP/master/nodemcu%20neopixel%20ldr_bb.jpg "Connections")
