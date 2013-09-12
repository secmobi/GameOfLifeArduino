# About
This is a DIY hardware to display the Game of Life. The screen is a 32x16 RGB LED matrix panel, while the controler is an Arduino Uno board. The code here is for Arduino to drive the LED matrix. If you've got proper boards, you can build the same toy in an hour by using our code.

# Hardware
* Arduino Uno x1
* Adafruit 32x16 LED matrix panel x1
* Dupont line x16
* Arduino Sensor Shield V5.0 x1 (optional)
* 6-9V 1A DC power adapter x1 (optional)
* LM35 temperature sensor x1 (optional)
* Button x1 (optional)

I strongly recomment you to choose all optional items, which will made the toy more stable and interesting. 

# Configuration
1. To connect the LED matrix with the sensor shield by Dupont lines, please refer: <http://learn.adafruit.com/32x16-32x32-rgb-led-matrix> ;
2. Connect the LM35 sensor with the A5 analog port;
3. Connect the button with the 12 digial port;
4. Connect the DC power adapter to Arduino Uno.

# Details
1. The Arduino Uno has only 2KB SRAM space. So we use each 1 byte to store 4 LEDs' status. This will make our code a little strange.
2. The Arduino itself cann't provide enough random numbers in the sense of each time it boot. So we use a LM35 temperature sensor to provider a environment related random seed value. We found this is enough to made the initial status different.
3. If the game reach a stable status or loop, just press button for half second, it will re-init again.

# Author
Claud Xiao <secmobi@gmail.com>
