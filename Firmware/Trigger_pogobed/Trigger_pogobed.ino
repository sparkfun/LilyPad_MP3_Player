// "Trigger" example sketch for Lilypad MP3 Player
// Mike Grusin, SparkFun Electronics
// http://www.sparkfun.com

// "Trigger" sketch modified for SFE pogobed testing:
// 1. RGB LED changes color R-G-B when turned
// 2. Pressing button causes RGB LED to turn off while held down.

// Note that initialization problems e.g. missing SD card
// will result in a slow red blink (number of blinks = error code,
// see MP3 Library source for info.

// ----

// This sketch (which is preloaded onto the board by default),
// will play a specific audio file when one of the five trigger
// inputs (labeled T1 - T5) is momentarily grounded.

// You can place up to five audio files on the micro-SD card.
// These files should have the desired trigger number (1 to 5)
// as the first character in the filename. The rest of the
// filename can be anything you like. Long file names will work,
// but will be translated into short 8.3 names. We recommend using
// 8.3 format names without spaces, but the following characters
// are OK: .$%'-_@~`!(){}^#&. The VS1053 can play a variety of 
// audio formats, see the datasheet for information.

// By default, a new trigger will interrupt a playing file, except
// itself. (In other words, a new trigger won't restart an 
// already-playing file). You can easily change this behavior by
// modifying the global variables "interrupt" and "interruptself"
// below.

// This sketch can output serial debugging information if desired
// by changing the global variable "debugging" to true. Note that
// this will take away trigger inputs 4 and 5, which are shared 
// with the TX and RX lines. You can keep these lines connected to
// trigger switches and use the serial port as long as the triggers
// are normally open (not grounded) and remain ungrounded while the
// serial port is in use.

// Uses the SdFat library by William Greiman, which is supplied
// with this archive, or download from http://code.google.com/p/sdfatlib/

// Uses the SFEMP3Shield library by Bill Porter, which is supplied
// with this archive, or download from http://www.billporter.info/

// License:
// We use the "beerware" license for our firmware. You can do
// ANYTHING you want with this code. If you like it, and we meet
// someday, you can, but are under no obligation to, buy me a
// (root) beer in return.

// Have fun! 
// -your friends at SparkFun

// Revision history:
// 1.0 initial release MDG 2013/03/11


// We'll need a few libraries to access all this hardware!

#include <SPI.h>            // To talk to the SD card and MP3 chip
#include <SdFat.h>          // SD card file system
#include <SFEMP3Shield.h>   // MP3 decoder chip
#include <PinChangeInt.h>   // Pin change interrupt (rotary enc.)

// Constants for the trigger input pins, which we'll place
// in an array for convenience:

const int TRIG1 = A0;
const int TRIG2 = A4;
const int TRIG3 = A5;
const int TRIG4 = 1;
const int TRIG5 = 0;
int trigger[5] = {TRIG1,TRIG2,TRIG3,TRIG4,TRIG5};

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

// Create library objects:

SFEMP3Shield MP3player;
SdFat sd;

// Set debugging = true if you'd like status messages sent 
// to the serial port. Note that this will take over trigger
// inputs 4 and 5. (You can leave triggers connected to 4 and 5
// and still use the serial port, as long as you're careful to
// NOT ground the triggers while you're using the serial port).

boolean debugging = false;

// Set interrupt = false if you would like a triggered file to
// play all the way to the end. If this is set to true, new
// triggers will stop the playing file and start a new one.

boolean interrupt = true;

// Set interruptself = true if you want the above rule to also
// apply to the same trigger. In other words, if interrupt = true
// and interruptself = false, subsequent triggers on the same
// file will NOT start the file over. However, a different trigger
// WILL stop the original file and start a new one.

boolean interruptself = false;

// We'll store the five filenames as arrays of characters.
// "Short" (8.3) filenames are used, followed by a null character.

char filename[5][13];

// Global variables and flags for interrupt request functions:

volatile int rotary_counter = 0; // Current "position" of rotary encoder (increments CW) 
volatile boolean rotary_change = false; // Will turn true if rotary_counter has changed
volatile boolean rotary_direction; // Direction rotary encoder was turned (true = CW)
volatile boolean button_pressed = false; // Will turn true if the button has been pushed
volatile boolean button_released = false; // Will turn true if the button has been released (sets button_downtime)
volatile unsigned long button_downtime = 0L; // ms the button was pushed before release

unsigned char ledcolor;

void setup()
{
  int x, index;
  SdFile file;
  byte result;
  char tempfilename[13];

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

  ledcolor = OFF;
  setLEDcolor(ledcolor);

  // The board uses a single I/O pin to select the
  // mode the MP3 chip will start up in (MP3 or MIDI),
  // and to enable/disable the amplifier chip:
  
  pinMode(SHDN_GPIO1,OUTPUT);
  digitalWrite(SHDN_GPIO1,LOW);  // MP3 mode / amp off


  // If debugging is true, initialize the serial port:
  // (The 'F' stores constant strings in flash memory to save RAM)
  
  if (debugging)
  {
    Serial.begin(9600);
    Serial.println(F("Lilypad MP3 Player trigger sketch"));
  }
  
  // Initialize the SD card; SS = pin 9, half speed at first

  if (debugging) Serial.print(F("initialize SD card... "));

  result = sd.begin(SD_CS, SPI_HALF_SPEED); // 1 for success
  
  if (result != 1) // Problem initializing the SD card
  {
    if (debugging) Serial.print(F("error, halting"));
    errorBlink(1); // Halt forever, blink LED if present.
  }
  else
    if (debugging) Serial.println(F("success!"));
  
  // Start up the MP3 library

  if (debugging) Serial.print(F("initialize MP3 chip... "));

  result = MP3player.begin(); // 0 or 6 for success

  // Check the result, see the library readme for error codes.

  if ((result != 0) && (result != 6)) // Problem starting up
  {
    if (debugging)
    {
      Serial.print(F("error code "));
      Serial.print(result);
      Serial.print(F(", halting."));
    }
    errorBlink(result); // Halt forever, blink red LED if present.
  }
  else
    if (debugging) Serial.println(F("success!"));

  // Now we'll access the SD card to look for any (audio) files
  // starting with the characters '1' to '5':

  if (debugging) Serial.println(F("reading root directory"));

  // Start at the first file in root and step through all of them:

  sd.chdir("/",true);
  while (file.openNext(sd.vwd(),O_READ))
  {
    // get filename

    file.getFilename(tempfilename);

    // Does the filename start with char '1' through '5'?      

    if (tempfilename[0] >= '1' && tempfilename[0] <= '5')
    {
      // Yes! subtract char '1' to get an index of 0 through 4.

      index = tempfilename[0] - '1';
      
      // Copy the data to our filename array.

      strcpy(filename[index],tempfilename);
  
      if (debugging) // Print out file number and name
      {
        Serial.print(F("found a file with a leading "));
        Serial.print(index+1);
        Serial.print(F(": "));
        Serial.println(filename[index]);
      }
    }
    else
      if (debugging)
      {
        Serial.print(F("found a file w/o a leading number: "));
        Serial.println(tempfilename);
      }
      
    file.close();
  }

  if (debugging)
    Serial.println(F("done reading root directory"));
  
  if (debugging) // List all the files we saved:
  {
    for(x = 0; x <= 4; x++)
    {
      Serial.print(F("trigger "));
      Serial.print(x+1);
      Serial.print(F(": "));
      Serial.println(filename[x]);
    }
  }

  // Set the VS1053 volume. 0 is loudest, 255 is lowest (off):

  MP3player.setVolume(40,40);
  
  // Turn on the amplifier chip:
  
  digitalWrite(SHDN_GPIO1,HIGH);
  delay(2);
  
  attachInterrupt(1,rotaryIRQ,CHANGE);
  PCintPort::attachInterrupt(ROT_SW, &buttonIRQ, CHANGE);

  ledcolor = RED; // will turn green when loop runs 
  setLEDcolor(ledcolor);
}


void loop()
{
  int t;              // current trigger
  static int last_t;  // previous (playing) trigger
  int x;
  byte result;
  
  // Step through the trigger inputs, looking for LOW signals.
  // The internal pullup resistors will keep them HIGH when
  // there is no connection to the input.

  // If serial debugging is on, only check triggers 1-3,
  // otherwise check triggers 1-5.

  for(t = 1; t <= (debugging ? 3 : 5); t++)
  {
    // The trigger pins are stored in the inputs[] array.
    // Read the pin and check if it is LOW (triggered).

    if (digitalRead(trigger[t-1]) == LOW)
    {
      // Wait for trigger to return high for a solid 50ms
      // (necessary to avoid switch bounce on T2 and T3
      // since we need those free for I2C control of the
      // amplifier)
      
      x = 0;
      while(x < 50)
      {
        if (digitalRead(trigger[t-1]) == HIGH)
          x++;
        else
          x = 0;
        delay(1);
      } 
        
      if (debugging)
      {
        Serial.print(F("got trigger "));
        Serial.println(t);
      }

      // Do we have a valid filename for this trigger?
      // (Invalid filenames will have 0 as the first character)

      if (filename[t-1][0] == 0)
      {
        if (debugging)
          Serial.println(F("no file with that number"));
      }
      else // We do have a filename for this trigger!
      {
        // If a file is already playing, and we've chosen to
        // allow playback to be interrupted by a new trigger,
        // stop the playback before playing the new file.

        if (interrupt && MP3player.isPlaying() && ((t != last_t) || interruptself))
        {
          if (debugging)
            Serial.println(F("stopping playback"));

          MP3player.stopTrack();
        }

        // Play the filename associated with the trigger number.
        // (If a file is already playing, this command will fail
        //  with error #2).

        result = MP3player.playMP3(filename[t-1]);

        if (result == 0) last_t = t;  // Save playing trigger
  
        if(debugging)
        {
          if(result != 0)
          {
            Serial.print(F("error "));
            Serial.print(result);
            Serial.print(F(" when trying to play track "));
          }
          else
          {
            Serial.print(F("playing "));
          }
          Serial.println(filename[t-1]);
        }
      }
    }
  }
  
  if (rotary_change)
  {
    if (ledcolor == RED) ledcolor = GREEN;
    else
    if (ledcolor == GREEN) ledcolor = BLUE;
    else
    if (ledcolor == BLUE) ledcolor = RED;
    setLEDcolor(ledcolor);
    rotary_change = false;
  }
  
  if (button_pressed)
  {
    setLEDcolor(OFF);
  }

  if (button_released)
  {
    button_pressed = false;
    button_released = false;
    setLEDcolor(ledcolor);
  }
}



void errorBlink(int blinks)
{
  // The following function will blink the red LED in the rotary
  // encoder (optional) a given number of times and repeat forever.
  // This is so you can see any startup error codes without having
  // to use the serial monitor window.

  int x;

  while(true) // Loop forever
  {
    for (x=0; x < blinks; x++) // Blink the given number of times
    {
      setLEDcolor(RED);
      delay(250);
      setLEDcolor(OFF);
      delay(250);
    }
    delay(1500); // Longer pause between blink-groups
  }
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

void setLEDcolor(unsigned char color)
{
  // Set the RGB LED in the (optional) rotary encoder
  // to a specific color. See the color #defines at the
  // start of this sketch.

  digitalWrite(ROT_LEDR,color & B001);
  digitalWrite(ROT_LEDG,color & B010);
  digitalWrite(ROT_LEDB,color & B100);  
}
