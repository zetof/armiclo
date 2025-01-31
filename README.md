# Arduino Midi Clock

This little project includes software and hardware implementations to achieve a compact and simple midi clock generator.

## Schematics

### LCD Keypad Shield

LCD 2x16 used to display status and commands. Buttons are used to control functionalities of the clock generator.

<img src="lcd_16x2.png" alt="LCD Keypad Shield" width="200"/>

### Rotary BPM Selector

Rotary resitor of 10kΩ connected to analog input `A1` used to control BPM rate between 40 and 300.

<img src="z-base_rotary.png" alt="Rotary BPM Selector" width="200"/>

### Beat LED Indicator

Red LED with 220Ω resistor, flashes at every beat.

<img src="z-base_led.png" alt="Beat LED Indicator" width="200"/>

### MIDI Output

A small 3.5mm jack output to send out MIDI clock stream, together with 220Ω resistors.

<img src="z-base_jack.png" alt="MIDI Output" width="200"/>
