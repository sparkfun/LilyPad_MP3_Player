// "Rotary_Encoder_Demo"
// Example sketch for the Lilypad MP3 Player
// Mike Grusin, SparkFun Electronics
// http://www.sparkfun.com

// This sketch demonstrates (only) the use of the rotary encoder
// hardware on the Lilypad MP3 Player board from Sparkfun. If you
// want to use the rotary encoder in your own sketches, this code
// will give you a starting point.

// HARDWARE CONNECTIONS

// This sketch will run on a LilyPad MP3 Player board (with
// a rotary encoder installed) without modifications.

// If you're not using the LilyPad MP3 Player board, connect
// a rotary encoder to your Arduino using these pins:

// Rotary encoder pin A to digital pin 3*
// Rotary encoder pin B to analog pin 3
// Rotary encoder pin C to ground

// This sketch implements software debounce, but you can further
// improve performance wby placing 0.1uF capacitors between
// A and ground, and B and ground.

// If you wish to use the RGB LED and button functions of 
// SparkFun part number COM-10982, use the following connections:

// Rotary encoder pin 1 (red cathode) to digital pin 10
// Rotary encoder pin 2 (green cathode) to analog pin 1
// Rotary encoder pin 3 (button) to digital pin 4
// Rotary encoder pin 4 (blue cathode) to digital pin 5
// Rotary encoder pin 5 (common anode) to VCC (3.3V or 5V)

// Note that because this is a common anode device,
// the pushbutton requires an external 1K-10K pullDOWN resistor
// to operate.

// * Pins marked with an asterisk should not change because
// they use interrupts on that pin. All other pins can change,
// see the constants below.

// SERIAL MONITOR

// Run this sketch with the serial monitor window set to 9600 baud.

// HOW IT WORKS

// The I/O pins used by the rotary encoder hardware are set up to 
// automatically call interrupt functions (rotaryIRQ and buttonIRQ)
// each time the rotary encoder changes states.

// The rotaryIRQ function transparently maintains a counter that
// increments or decrements by one for each detent ("click") of
// the rotary encoder knob. This function also sets a flag
// (rotary_change) to true whenever the counter changes. You can
// check this flag in your main loop() code and perform an action
// when the knob is turned.

// The buttonIRQ function does the same thing for the pushbutton
// built into the rotary encoder knob. It will set flags for 
// button_pressed and button_released that you can monitor in your
// main loop() code. There is also a variable for button_downtime
// which records how long the button was held down.

// There is also code in the main loop() that keeps track
// of whether the button is currently being held down and for
// how long. This is useful for "hold button down for five seconds
// to power off"-type situations, which cannot be handled by
// interrupts alone because no interrupts will be called until
// the button is actually released.

// Uses the PinChangeInt library by Lex Talionis, 
// download from http://code.google.com/p/arduino-pinchangeint/

// License:
// We use the "beerware" license for our firmware. You can do
// ANYTHING you want with this code. If you like it, and we meet
// someday, you can, but are under no obligation to, buy me a
// (root) beer in return.

// Have fun! 
// -your friends at SparkFun

// Revision history:
// 1.0 initial release MDG 2013/01/24

// Load the PinChangeInt (pin change interrupt) library

#include <PinChangeInt.h>

// LilyPad MP3 pin defines. Not all of these are used in this
// sketch; the unused pins are commented out:

//#define TRIG1 A0
#define ROT_LEDG A1     // green LED
//#define SHDN_GPIO1 A2
#define ROT_B A3        // rotary B
//#define TRIG2_SDA A4
//#define TRIG3_SCL A5
//#define RIGHT A6
//#define LEFT A7

//#define TRIG5_RXI 0
//#define TRIG4_TXO 1
//#define MP3_DREQ 2
#define ROT_A 3          // rotary A
#define ROT_SW 4         // rotary puhbutton
#define ROT_LEDB 5       // blue LED
//#define MP3_CS 6
//#define MP3_DCS 7
//#define MP3_RST 8
//#define SD_CS 9
#define ROT_LEDR 10      // red LED
//#define MOSI 11
//#define MISO 12
//#define SCK 13

// RGB LED colors (for common anode LED, 0 is on, 1 is off)

#define OFF B111
#define RED B110
#define GREEN B101
#define YELLOW B100
#define BLUE B011
#define PURPLE B010
#define CYAN B001
#define WHITE B000

// Global variables that can be changed in interrupt routines

volatile int rotary_counter = 0; // current "position" of rotary encoder (increments CW) 
volatile boolean rotary_change = false; // will turn true if rotary_counter has changed
volatile boolean button_pressed = false; // will turn true if the button has been pushed
volatile boolean button_released = false; // will turn true if the button has been released (sets button_downtime)
volatile unsigned long button_downtime = 0L; // ms the button was pushed before release


void setup() 
{
  // Set up all the I/O pins. Unused pins are commented out.

  //  pinMode(TRIG1, INPUT);
  //  digitalWrite(TRIG1, HIGH); // turn on weak pullup
  //  pinMode(MP3_CS, OUTPUT);
  //  pinMode(SHDN_GPIO1, OUTPUT);
  pinMode(ROT_B, INPUT);
  digitalWrite(ROT_B, HIGH); // turn on weak pullup
  //  pinMode(TRIG2_SDA, INPUT);
  //  digitalWrite(TRIG1, HIGH); // turn on weak pullup
  //  pinMode(TRIG3_SCL, INPUT);
  //  digitalWrite(TRIG1, HIGH); // turn on weak pullup
  //  pinMode(TRIG5_RXI, INPUT);
  //  digitalWrite(TRIG5_RXI, HIGH); // turn on weak pullup
  //  pinMode(TRIG4_TXO, INPUT);
  //  digitalWrite(TRIG4_TXO, HIGH); // turn on weak pullup
  pinMode(ROT_A, INPUT);
  digitalWrite(ROT_A, HIGH); // turn on weak pullup
  pinMode(ROT_SW, INPUT);
  // The rotary switch is common anode with external pulldown, do not turn on pullup
  //  pinMode(MP3_DREQ, INPUT);
  pinMode(ROT_LEDB, OUTPUT);
  pinMode(ROT_LEDG, OUTPUT);
  //  pinMode(MP3_DCS, OUTPUT);
  //  pinMode(MP3_RST, OUTPUT);
  pinMode(ROT_LEDR, OUTPUT);
  //  pinMode(SD_CS, OUTPUT);
  //  pinMode(MOSI, OUTPUT);
  //  pinMode(MISO, INPUT);
  //  pinMode(SCK, OUTPUT);

  //digitalWrite(SHDN_GPIO1, LOW); // Shut down Amplifier chip
  
  //digitalWrite(MP3_RST, LOW); // Shut down MP3 chip
  
  setLED(OFF);

  Serial.begin(9600); // Use serial for debugging 
  Serial.println("rotary encoder testing");

  // We use the standard external interrupt pin for the rotary,
  // but we'll use the pin change interrupt library for the button.
  
  attachInterrupt(1,rotaryIRQ,CHANGE);
  PCintPort::attachInterrupt(ROT_SW, &buttonIRQ, CHANGE);
}


void buttonIRQ()
{
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
  // Process input from the rotary encoder.
  // The rotary "position" is held in rotary_counter, increasing for CW rotation (changes by one per detent).
  // If the position changes, rotary_change will be set true. (You may manually set this to false after handling the change).

  // This function will automatically run when rotary encoder input A transitions in either direction (low to high or high to low)
  // By saving the state of the A and B pins through two interrupts, we'll determine the direction of rotation
  // int rotary_counter will be updated with the new value, and boolean rotary_change will be true if there was a value change
  // Based on concepts from Oleg at circuits@home (http://www.circuitsathome.com/mcu/rotary-encoder-interrupt-service-routine-for-avr-micros)
  // Unlike Oleg's original code, this code uses only one interrupt and has only two transition states;
  // it has less resolution but needs only one interrupt, is very smooth, and handles switchbounce well.
  
  static unsigned char rotary_state = 0; // current and previous encoder states
  
  rotary_state <<= 2;  // remember previous state
  rotary_state |= (digitalRead(ROT_A) | (digitalRead(ROT_B) << 1));  // mask in current state
  rotary_state &= 0x0F; // zero upper nybble

  //Serial.println(rotary_state,HEX);

  if (rotary_state == 0x09) // from 10 to 01, increment counter. Also try 0x06 if unreliable
  {
    rotary_counter++;
    rotary_change = true;
  }
  else if (rotary_state == 0x03) // from 00 to 11, decrement counter. Also try 0x0C if unreliable
  {
    rotary_counter--;
    rotary_change = true;
  }
}


void loop()
{
  // "Static" variables are initalized once the first time
  // that loop runs, but they keep their values through
  // successive loops.
  
  static unsigned char x = 0;
  static boolean button_down = false;
  static unsigned long int button_down_start, button_down_time;
  
  // The rotary IRQ sets the flag rotary_counter to true
  // if the knob position has changed. We can use this flag
  // to do something in the main loop() each time there's
  // a change. We'll clear this flag when we're done, so 
  // that we'll only do this if() once for each change.
  
  if (rotary_change)
  {
    Serial.print("rotary: ");
    Serial.println(rotary_counter,DEC);
    rotary_change = false; // Clear flag
  }

  // The button IRQ also sets flags to true, one for
  // button_pressed, one for button_released. Like the rotary
  // flag, we'll clear these when we're done handling them.

  if (button_pressed)
  {
    Serial.println("button press");
    x++; setLED(x); // Change the color of the knob LED
    button_pressed = false; // Clear flag

    // We'll set another flag saying the button is now down
    // this is so we can keep track of how long the button
    // is being held down. (We can't do this in interrupts,
    // because the button state is not changing).

    button_down = true;
    button_down_start = millis();
  }

  if (button_released)
  {
    Serial.print("button release, downtime: ");
    Serial.println(button_downtime,DEC);
    x++; setLED(x); // Change the color of the knob LED
    button_released = false; // Clear flag

    // Clear our button-being-held-down flag

    button_down = false;
  }

  // Now we can keep track of how long the button is being
  // held down, and perform actions based on that time.
  // This is useful for "hold down for five seconds to power off"
  // -type functions.

  if (button_down)
  {
    button_down_time = millis() - button_down_start;
    // Serial.println(button_down_time);
    if (button_down_time > 1000)
      Serial.println("button held down for one second");
  }
}


void setLED(unsigned char color)
// Set RGB LED to one of eight colors (see #defines above)
{
  digitalWrite(ROT_LEDR,color & B001);
  digitalWrite(ROT_LEDG,color & B010);
  digitalWrite(ROT_LEDB,color & B100);  
}



