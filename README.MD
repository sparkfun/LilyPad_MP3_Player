SparkFun LilyPad MP3
========================================

[![SparkFun LilyPad MP3](https://cdn.sparkfun.com//assets/parts/6/2/9/8/11013-01a.jpg)](https://www.sparkfun.com/products/11013)

[*SparkFun LilyPad MP3 (DEV-11013)*](https://www.sparkfun.com/products/11013)

The LilyPad MP3 Player is an amazing little board that contains everything you need to play almost any audio file you wish. You can use it to create MP3-playing hoodies, talking teddy bears, or anything else your imagination can come up with!

### Installation:

You should install the latest version of the Arduino IDE, available at http://www.arduino.cc.

This software requires several nonstandard libraries:
	SdFat (by William Greiman)
	SFEMP3Shield (by Bill Porter)
	pinChangeInt (by Lex Talionis)
	
	The current version of these libraries as of this date are included in this archive.

**TL;DR:** The one-step procedure is to drag the contents of the Arduino folder to your personal Arduino sketch folder. This will create a "libraries" folder (containing support libraries), and a "LilyPad MP3 Player" folder (containing example sketches).

To install the libraries manually, copy the "libraries" folder to your personal Arduino sketch directory. (If there is already a "libraries" folder there, go ahead and add the included libraries to it.) You'll need to restart the Arduino IDE to get it to recognize the new libraries.

To install the example sketches, copy the "LilyPad MP3 Player" folder to your personal Arduino sketch folder.

Loading the sketches:

To load a sketch onto the LilyPad MP3 Player:

1. Connect the LilyPad MP3 Player to your computer using a 5V FTDI board or cable (available at http://www.sparkfun.com). Turn the LilyPad MP3 Player ON. The red LED should light up. Note that a Lipo battery or external power source is required to run the LilyPad MP3 Player.

2. Start the Arduino IDE.

3. Select the correct board type from the menu: Tools / Board / "Arduino Pro or Pro Mini (3.3V, 8MHz) w/ATmega328"

4. Select the correct serial port from the menu: Tools / Serial Port / (the port that your FTDI board or cable has created, usually the highest number)

5. Load the desired sketch into the Arduino IDE.

6. Click on the Upload (right arrow) button at the top of the window. The code should compile then upload to the LilyPad MP3 Player. If you have errors, double-check that you installed the required libraries to the correct place, and restarted the Arduino IDE.

See the tutorials at www.sparkfun.com for more information on using and hacking the LilyPad MP3 Player.

Documentation
--------------
* **[Installing an Arduino Library Guide](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)** - Basic information on how to install an Arduino library.
* **[Hookup Guide](https://learn.sparkfun.com/tutorials/getting-started-with-the-lilypad-mp3-player)** - Basic hookup guide.

License Information
-------------------

This product is _**open source**_! 

Please review the LICENSE.md file for license information. 

If you have any questions or concerns on licensing, please contact support@sparkfun.com.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.

_<COLLABORATION CREDIT>_
