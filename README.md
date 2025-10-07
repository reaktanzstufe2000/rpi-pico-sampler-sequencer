# Raspberry Pi Pico Sampler Sequencer

## Project Overview

**This is a work in progress! More accurately this is a little holiday project to try some AI assited coding. Chances are that I will stop working on that eventually ;)**

Sampler-sequencer based on Raspberry Pi Pico. Loaded it with tasty samples from Crates Motel 909 kit (https://youtu.be/H9J_VnYfAco?si=cDU6HVoU61Lqj7db) but nothing is stopping you from using your own. Use Audacity or similar to change sample rate to 22.05 kHz and reduce bitrate to 16 bit signed integer. Then somehow bring that into the c array format. :D
I am providing this lovely project here for free so there are no guarantees or whatever. Just take what you can get here. If anybody feels the call to branch this and develop it further --> go for it!

## You need

- 16x2 I2C backpack LCD display  
- rotary encoder for menu navigation and parameter control
- PCM5102 I2S DAC for high-quality stereo audio output  
- Additional buttons for triggering samples and controlling playback --> tbd


## Wiring Table
**Will add a photo as soon as I have my prototype together.**

| Component             | Component Pin   | Pico Pin (Board #) |
|----------------------|-----------------|---------------------|
| I2C 16x2 LCD          | GND             | any GND pin         |
|                       | VCC             | VBUS (Pin 40)       |
|                       | SDA             | GP4 (Pin 6)         |
|                       | SCL             | GP5 (Pin 7)         |
| Rotary Encoder        | CLK             | GP14 (Pin 19)       |
|                       | DT              | GP15 (Pin 20)       |
|                       | SW              | GP16 (Pin 21)       |
|                       | +               | VSYS (Pin 36)       |
|                       | GND             | any GND pin         |
| PCM5102 DAC (I2S)     | SCK             | any GND pin         |
|                       | BCK             | GP11 (Pin 31)       |
|                       | DIN             | GP10 (Pin 34)       |
|                       | LCK             | GP12 (Pin 32)       |
|                       | GND             | any GND pin         |
|                       | VIN             | VSYS (Pin 40)       |
| Buttons               | Button 1        | TBD                 |
|                       | Button 2        | TBD                 |
|                       | Button 3        | TBD                 |

The PCM5102 DAC has this XSMT pin that either needs to be pulled high by connecting it to 3.3V or soldring the correct solderbridge.

## Libs used

- Raspberry Pi Pico Arduino core by earlephilhower (https://github.com/earlephilhower/arduino-pico)
- PicoLCD_I2C by smlaage (https://github.com/smlaage/PicoLCD_I2C)

## License

This project is open-source under the MIT License. See `LICENSE` for details.