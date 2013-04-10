// "Player" example sketch for Lilypad MP3 Player
// Mike Grusin, SparkFun Electronics
// http://www.sparkfun.com

// This sketch turns the Lilypad MP3 Player into a basic MP3
// player that you can control with the optional rotary encoder.

// HARDWARE

// To use this sketch, an optional RGB rotary encoder must be soldered
// to the MP3 Player board. The board is designed to hold SparkFun
// part number COM-10892. https://www.sparkfun.com/products/10982

// SOFTWARE

// This sketch requires the following libraries. These are included
// in the Lilypad MP3 Player firmware zip file and must be copied to
// a "libraries" folder in your Arduino sketch directory:

// Uses the SdFat library by William Greiman, which is supplied
// with this archive, or download from http://code.google.com/p/sdfatlib/

// Uses the SFEMP3Shield library by Porter and Flaga, which is supplied
// with this archive, or download from http://www.billporter.info/

// Uses the PinChangeInt library by Lex Talionis, which is supplied 
// with this archive, or download from http://code.google.com/p/arduino-pinchangeint/

// BASIC OPERATION:

// Place your audio files in the root directory of the SD card.
// Your files MUST have one of the following extensions: MP3, WAV,
// MID (MIDI), MP4, WMA, AAC, FLA, OGG. Note that this is solely to
// prevent the VS1053 from locking up from being fed non-audio data
// (files without one of the above extensions are quietly skipped).
// You can rename any playable file to any of the above extensions,
// or add additional extensions to the isPlayable() function below.
// See the VS1053 datasheet for the audio file types it can play.

// The player has two modes, TRACK and VOLUME. In TRACK mode, turning
// the knob will move to the next or previous track. In VOLUME mode,
// turning the knob will increase or decrease the volume.

// You can tell what mode you're in by the color of the RGB LED in the
// knob of the rotary encoder. TRACK mode is red, VOLUME mode is green.

// To switch between modes, hold down the button on the rotary encoder
// until the color changes (more than one second).

// To start and stop playback, press the button on the rotary encoder
// *quickly* (less than one second). When the player is playing, it
// will stubbornly keep playing; starting new tracks when the previous
// one ends, and switching to new tracks if you turn the knob in TRACK
// mode. When the player is stopped, it will not start playing until
// you press the button *quickly*, but it will silently change tracks
// or adjust the volume if you turn the knob.

// SERIAL DEBUGGING

// This sketch can output serial debugging information if desired
// by changing the global variable "debugging" to true. Note that
// this will take away trigger inputs 4 and 5, which are shared 
// with the TX and RX lines. You can keep these lines connected to
// trigger switches and use the serial port as long as the triggers
// are normally open (not grounded) and remain ungrounded while the
// serial port is in use.

// License:
// We use the "beerware" license for our firmware. You can do
// ANYTHING you want with this code. If you like it, and we meet
// someday, you can, but are under no obligation to, buy me a
// (root) beer in return.

// Have fun! 
// -your friends at SparkFun

// Revision history:
// 1.0 initial release MDG 2013/1/31

// Required libraries:

#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>
#include <PinChangeInt.h>

// Set debugging to true to get serial messages:

boolean debugging = true;

// Possible modes (first and last are there to make
// rotating through them easier):

#define FIRST_MODE 0
#define TRACK 0
#define VOLUME 1
#define LAST_MODE 1

// Initial mode for the rotary encoder. TRACK lets you
// select audio tracks, VOLUME lets you change the volume.
// In any mode, a quick press will start and stop playback.
// A longer press will switch to the next mode.

unsigned char rotary_mode = TRACK;

// Initial volume for the MP3 chip. 0 is the loudest, 255
// is the lowest.

unsigned char volume = 40;

// Start up *not* playing:

boolean playing = false;

// Set loop_all to true if you would like to automatically
// start playing the next file after the current one ends:

boolean loop_all = true;

// LilyPad MP3 pin definitions:

#define TRIG1 A0
#define ROT_LEDG A1
#define SHDN_GPIO1 A2
#define ROT_B A3
#define TRIG2_SDA A4
#define TRIG3_SCL A5
#define RIGHT A6
#define LEFT A7

#define TRIG5_RXI 0
#define TRIG4_TXO 1
#define MP3_DREQ 2
#define ROT_A 3
#define ROT_SW 4
#define ROT_LEDB 5
#define MP3_CS 6
#define MP3_DCS 7
#define MP3_RST 8
#define SD_CS 9
#define ROT_LEDR 10
#define MOSI 11
#define MISO 12
#define SCK 13

// RGB LED colors (for common anode LED, 0 is on, 1 is off)

#define OFF B111
#define RED B110
#define GREEN B101
#define YELLOW B100
#define BLUE B011
#define PURPLE B010
#define CYAN B001
#define WHITE B000

// Global variables and flags for interrupt request functions:

volatile int rotary_counter = 0; // Current "position" of rotary encoder (increments CW) 
volatile boolean rotary_change = false; // Will turn true if rotary_counter has changed
volatile boolean rotary_direction; // Direction rotary encoder was turned (true = CW)
volatile boolean button_pressed = false; // Will turn true if the button has been pushed
volatile boolean button_released = false; // Will turn true if the button has been released (sets button_downtime)
volatile unsigned long button_downtime = 0L; // ms the button was pushed before release
char track[13];

// Library objects:

SdFat sd;
SdFile file;
SFEMP3Shield MP3player;


void setup()
{
  byte result;

  if (debugging)
  {
    Serial.begin(9600);
    Serial.println(F("Lilypad MP3 Player"));

    // ('F' places constant strings in program flash to save RAM)

    Serial.print(F("Free RAM = "));
    Serial.println(FreeRam(), DEC);
  }

  // Set up I/O pins:

  pinMode(TRIG1, INPUT);
  digitalWrite(TRIG1, HIGH); // turn on weak pullup
  pinMode(MP3_CS, OUTPUT);
  pinMode(SHDN_GPIO1, OUTPUT);
  pinMode(ROT_B, INPUT);
  digitalWrite(ROT_B, HIGH); // turn on weak pullup
  pinMode(TRIG2_SDA, INPUT);
  digitalWrite(TRIG1, HIGH); // turn on weak pullup
  pinMode(TRIG3_SCL, INPUT);
  digitalWrite(TRIG1, HIGH); // turn on weak pullup
  pinMode(TRIG5_RXI, INPUT);
  digitalWrite(TRIG5_RXI, HIGH); // turn on weak pullup
  pinMode(TRIG4_TXO, INPUT);
  digitalWrite(TRIG4_TXO, HIGH); // turn on weak pullup
  pinMode(ROT_A, INPUT);
  digitalWrite(ROT_A, HIGH); // turn on weak pullup
  pinMode(ROT_SW, INPUT);
  // switch is common anode with external pulldown, do not turn on pullup
  pinMode(MP3_DREQ, INPUT);
  pinMode(ROT_LEDB, OUTPUT);
  pinMode(ROT_LEDG, OUTPUT);
  pinMode(MP3_DCS, OUTPUT);
  pinMode(MP3_RST, OUTPUT);
  pinMode(ROT_LEDR, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);

 // Turn off amplifier chip / turn on MP3 mode:

  digitalWrite(SHDN_GPIO1, LOW);
  setLEDcolor(OFF);

  // Initialize the SD card:

  if (debugging) Serial.println(F("Initializing SD card... "));

  result = sd.begin(SD_SEL, SPI_HALF_SPEED);

  if (result != 1)
  {
    if (debugging) Serial.println(F("error, halting"));
    errorBlink(1,RED);
  }
  else 
    if (debugging) Serial.println(F("OK"));
 
  //Initialize the MP3 chip:
  
  if (debugging) Serial.println(F("Initializing MP3 chip... "));

  result = MP3player.begin();

  // Check result, 0 and 6 are OK:
  
  if((result != 0) && (result != 6))
  {
    if (debugging)
    {
      Serial.print(F("error "));
      Serial.println(result);
    }
    errorBlink(result,BLUE);
  }
  else
    if (debugging) Serial.println(F("OK"));
  
  // Set up interrupts. We'll use the standard external interrupt
  // pin for the rotary, but we'll use the pin change interrupt
  // library for the button:

  attachInterrupt(1,rotaryIRQ,CHANGE);
  PCintPort::attachInterrupt(ROT_SW, &buttonIRQ, CHANGE);

  // Get initial track:
  
  sd.chdir("/",true); // Index beginning of root directory
  getNextTrack();
  if (debugging)
  {
    Serial.print(F("current track: "));
    Serial.println(track);
  }
  
  // Set initial volume (same for both left and right channels)
  
  MP3player.setVolume(volume, volume);
  
  // Initial mode for the rotary encoder

  LEDmode(rotary_mode);
  
  // Uncomment to get a directory listing of the SD card:
  // sd.ls(LS_R | LS_DATE | LS_SIZE);

  // Turn on amplifier chip:
  
  digitalWrite(SHDN_GPIO1, HIGH);
  delay(2);
}


void buttonIRQ()
{
  // Button press interrupt request function (IRQ).
  // This function is called *automatically* when the button
  // changes state.

  // Process rotary encoder button presses and releases, including
  // debouncing (extra "presses" from noisy switch contacts).

  // If button is pressed, the button_pressed flag is set to true.
  // (Manually set this to false after handling the change.)

  // If button is released, the button_released flag is set to true,
  // and button_downtime will contain the duration of the button
  // press in ms. (Set this to false after handling the change.)

  // Raw information from PinChangeInt library:
  
  // Serial.print("pin: ");
  // Serial.print(PCintPort::arduinoPin);
  // Serial.print(" state: ");
  // Serial.println(PCintPort::pinState);
  
  static boolean button_state = false;
  static unsigned long start, end;
    
  if ((PCintPort::pinState == HIGH) && (button_state == false)) 
  // Button was up, but is currently being pressed down
  {
    // Discard button presses too close together (debounce)
    start = millis();
    if (start > (end + 10)) // 10ms debounce timer
    {
      button_state = true;
      button_pressed = true;
    }
  }
  else if ((PCintPort::pinState == LOW) && (button_state == true))
  // Button was down, but has just been released
  {
    // Discard button releases too close together (debounce)
    end = millis();
    if (end > (start + 10)) // 10ms debounce timer
    {
      button_state = false;
      button_released = true;
      button_downtime = end - start;
    }
  }
}


void rotaryIRQ()
{
  // Rotary encoder interrupt request function (IRQ).
  // This function is called *automatically* when the
  // rotary encoder changes state.

  // Process input from the rotary encoder.
  // The rotary "position" is held in rotary_counter, increasing
  // for CW rotation (changes by one per detent).
  
  // If the position changes, rotary_change will be set true.
  // (You may manually set this to false after handling the change).

  // This function will automatically run when rotary encoder input A
  // transitions in either direction (low to high or high to low).
  // By saving the state of the A and B pins through two interrupts,
  // we'll determine the direction of rotation.
  
  // Int rotary_counter will be updated with the new value, and boolean
  // rotary_change will be true if there was a value change.
  
  // Based on concepts from Oleg at circuits@home (http://www.circuitsathome.com/mcu/rotary-encoder-interrupt-service-routine-for-avr-micros)
  // Unlike Oleg's original code, this code uses only one interrupt and
  // has only two transition states; it has less resolution but needs only
  // one interrupt, is very smooth, and handles switchbounce well.
  
  static unsigned char rotary_state = 0; // Current and previous encoder states
  
  rotary_state <<= 2;  // Remember previous state
  rotary_state |= (digitalRead(ROT_A) | (digitalRead(ROT_B) << 1));  // Mask in current state
  rotary_state &= 0x0F; // Zero upper nybble

  //Serial.println(rotary_state,HEX);

  if (rotary_state == 0x09) // From 10 to 01, increment counter. Also try 0x06 if unreliable.
  {
    rotary_counter++;
    rotary_direction = true;
    rotary_change = true;
  }
  else if (rotary_state == 0x03) // From 00 to 11, decrement counter. Also try 0x0C if unreliable.
  {
    rotary_counter--;
    rotary_direction = false;
    rotary_change = true;
  }
}


void loop()
{
  // "Static" variables are initalized once the first time
  // the loop runs, but they keep their values through
  // successive loops.
  
  static boolean button_down = false;
  static unsigned long int button_down_start, button_down_time;
  
  // rotaryIRQ() sets the flag rotary_counter to true
  // if the knob position has changed. We can use this flag
  // to do something in the main loop() each time there's
  // a change. We'll clear this flag when we're done, which
  // lets us run this if() once for each change.
  
  if (rotary_change)
  {
    switch (rotary_mode)
    {
      case TRACK:
      
        // Before switching to a new audio file, we MUST
        // stop playing before accessing the SD directory:
       
        if (playing)
          stopPlaying();
      
        // Get the next file:
      
        if (rotary_direction)
          getNextTrack();
        else
          getPrevTrack();
      
        // If we were previously playing, let's start again!
      
        if (playing) startPlaying(); 

        if (debugging)
        {
          Serial.print(F("current track "));
          Serial.println(track);
        }
        break;
      
      case VOLUME:

        // Change the volume. Easy.
        
        changeVolume(rotary_direction);
        break;
    }
    rotary_change = false; // Clear flag
  }

  // The button IRQ also sets several flags to true, one for
  // button_pressed, one for button_released. Like the rotary
  // flag, we'll clear these when we're done handling them:

  if (button_pressed)
  {
    if (debugging) Serial.println(F("button press"));

    button_pressed = false; // Clear flag

    // We'll set another flag saying the button is now down
    // this is so we can keep track of how long the button
    // is being held down. (We can't do this in interrupts,
    // because the button state doesn't change while it's
    // being held down):

    button_down = true;
    button_down_start = millis();
  }

  if (button_released)
  {
    if (debugging)
    {
      Serial.print(F("button release, downtime: "));
      Serial.println(button_downtime,DEC);
    }
    
    // For quick button presses, start or stop playback:
    
    if (button_downtime < 1000)
    {
      playing = !playing;
      if (playing)
        startPlaying();
      else
        stopPlaying();
    }

    button_released = false; // Clear flags
    button_down = false;
  }

  // Now we can keep track of how long the button is being
  // held down, and perform actions based on that time.
  // This lets us implement the "hold down for one second
  // to change mode" action:

  if (button_down)
  {
    button_down_time = millis() - button_down_start;

    if (button_down_time > 1000)
    {
      // Switch to next mode

      rotary_mode++;

      // Loop around if necessary

      if (rotary_mode > LAST_MODE) rotary_mode = FIRST_MODE;

      // Set LED to mode color

      LEDmode(rotary_mode);

      if (debugging)
      {
        Serial.print(F("mode "));
        Serial.println(rotary_mode);
      }
      
      // Reset counter so holding the button will continue
      // switching through modes:
      
      button_down_start = millis();
    }
  }
  
  // Handle "last track ended" situations
  // (should we play the next track?)
  
  // Are we in "playing" mode, and has the
  // current file ended?
  
  if (playing && !MP3player.isPlaying())
  {
    getNextTrack(); // Set up for next track
    
    // If loop_all is true, start the next track
    
    if (loop_all)
    {
      startPlaying();
    }
    else
      playing = false;
  }
}


void changeVolume(boolean direction)
{
  // Increment or decrement the volume.
  // This is handled internally in the VS1053 MP3 chip.
  // Lower numbers are louder (0 is the loudest).
  
  if (volume < 255 && direction == false)
    volume += 2;
  
  if (volume > 0 && direction == true)
    volume -= 2;

  MP3player.setVolume(volume, volume);

  if (debugging)
  {
    Serial.print(F("volume "));
    Serial.println(volume);
  }
}


void getNextTrack()
{
  // Get the next playable track (check extension to be
  // sure it's an audio file)
  
  do
    getNextFile();
  while(isPlayable() != true);
}


void getPrevTrack()
{
  // Get the previous playable track (check extension to be
  // sure it's an audio file)

  do
    getPrevFile();
  while(isPlayable() != true);
}


void getNextFile()
{
  // Get the next file (which may be playable or not)

  int result = (file.openNext(sd.vwd(), O_READ));

  // If we're at the end of the directory,
  // loop around to the beginning:
  
  if (!result)
  {
    sd.chdir("/",true);
    getNextTrack();
    return;
  }
  file.getFilename(track);  
  file.close();
}


void getPrevFile()
{
  // Get the previous file (which may be playable or not)
  
  char test[13], prev[13];

  // Getting the previous file is tricky, since you can
  // only go forward when reading directories.

  // To handle this, we'll save the name of the current
  // file, then keep reading all the files until we loop
  // back around to where we are. While doing this we're
  // saving the last file we looked at, so when we get
  // back to the current file, we'll return the previous
  // one.
  
  // Yeah, it's a pain.

  strcpy(test,track);

  do
  {
    strcpy(prev,track);
    getNextTrack();
  }
  while(strcasecmp(track,test) != 0);
  
  strcpy(track,prev);
}


void startPlaying()
{
  int result;
  
  if (debugging)
  {
    Serial.print(F("playing "));
    Serial.print(track);
    Serial.print(F("..."));
  }  

  result = MP3player.playMP3(track);

  if (debugging)
  {
    Serial.print(F(" result "));
    Serial.println(result);  
  }
}


void stopPlaying()
{
  if (debugging) Serial.println(F("stopping playback"));
  MP3player.stopTrack();
}


boolean isPlayable()
{
  // Check to see if a filename has a "playable" extension.
  // This is to keep the VS1053 from locking up if it is sent
  // unplayable data.

  char *extension;
  
  extension = strrchr(track,'.');
  extension++;
  if (
    (strcasecmp(extension,"MP3") == 0) ||
    (strcasecmp(extension,"WAV") == 0) ||
    (strcasecmp(extension,"MID") == 0) ||
    (strcasecmp(extension,"MP4") == 0) ||
    (strcasecmp(extension,"WMA") == 0) ||
    (strcasecmp(extension,"FLA") == 0) ||
    (strcasecmp(extension,"OGG") == 0) ||
    (strcasecmp(extension,"AAC") == 0)
  )
    return true;
  else
    return false;
}


void LEDmode(unsigned char mode)
{
  // Change the RGB LED to a specific color for each mode
  // (See #defines at start of sketch for colors.)

  switch (mode)
  {
    case TRACK:
      setLEDcolor(RED);
      break;
    case VOLUME:
      setLEDcolor(GREEN);
      break;
    default:
      setLEDcolor(OFF);
  }
}


void setLEDcolor(unsigned char color)
{
  // Set the RGB LED in the (optional) rotary encoder
  // to a specific color. See the color #defines at the
  // start of this sketch.

  digitalWrite(ROT_LEDR,color & B001);
  digitalWrite(ROT_LEDG,color & B010);
  digitalWrite(ROT_LEDB,color & B100);  
}


void errorBlink(int blinks, byte color)
{
  // This function will blink the RGB LED in the rotary encoder
  // (optional) a given number of times and repeat forever.
  // This is so you can see error codes without having to use
  // the serial monitor window.

  int x;

  while(true) // Loop forever
  {
    for (x=0; x < blinks; x++) // Blink a given number of times
    {
      setLEDcolor(color);
      delay(250);
      setLEDcolor(OFF);
      delay(250);
    }
    delay(1500); // Longer pause between blink-groups
  }
}

