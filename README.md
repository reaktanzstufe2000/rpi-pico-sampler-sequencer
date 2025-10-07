# Raspberry Pi Pico Sampler Sequencer

## Project Overview

Sampler-sequencer based on Raspberry Pi Pico. Loaded it with tasty samples from Crates Motel 909 kit (https://youtu.be/H9J_VnYfAco?si=cDU6HVoU61Lqj7db) but nothing is stopping you from using your own. Use Audacity or similar to change sample rate to 22.05 kHz and reduce bitrate to 16 bit signed integer. Then somehow bring that into the c array format. :D
I am providing this lovely project here for free so there are no guarantees or whatever. Just take what you can get here. I will probably add a few things but all in good time! If anybody feels the call to branch this and develop it further --> go for it!

## You need

- 16x2 I2C backpack LCD display  
- rotary encoder for menu navigation and parameter control
- PCM5102 I2S DAC for high-quality stereo audio output  
- Additional buttons for triggering samples and controlling playback --> tbd


## Wiring Table

| Component             | Component Pin   | Pico Pin        |
|----------------------|-----------------|-----------------|
| I2C 16x2 LCD          | GND             | any GND pin             |
|                      | VCC             | VBUS (40)             |
|                      | SDA             | I2C0 SDA (6)             |
|                      | SCL             | I2C0 SCL (7)              |
| Rotary Encoder        | A               |           |
|                      | B               |            |
|                      | Button          |           |
| PCM5102 DAC (I2S)     | DIN             |          |
|                      | BCLK            |            |
|                      | LRCLK           |            |
| Buttons  TBD            | Button 1        |             |
|                      | Button 2        |             |
|                      | Button 3        |            |

The PCM5102 DAC has this XSMT pin that either needs to be pulled high by connecting it to 3.3V or soldring the correct solderbridge.

## Libs used

- Raspberry Pi Pico Arduino core by Earl E. Philhower
- PicoLCD_I2C by smlaage

## License

This project is open-source under the MIT License. See `LICENSE` for details.