/* 
  SFE TPA2015D2 stereo class-D amplifier library
  Mike Grusin
  http://www.sparkfun.com

	This library provides a set of functions to control the TI TPA2016D2
	stereo amplifier via I2C.
	
	To use these functions, include the following libraries in your sketch: 
	
		#include <Wire.h>
		#include <SFE_TPA2016D2.h>
	
	Next, create a SFE_TPA2016D2 object. You can name this whatever you wish:

		SFE_TPA2016D2 amp;
	
	You can then call any of the following functions by prefacing it with
	your object name:
	
		amp.disableRightSpeaker();
	
	All functions return true if the read/write was successful, and false
	if there was a communications failure. You can ignore the return value
	if you just don't care anymore.
	
	For all functions that return a value, e.g. "readGain(char *gain)", first
	declare	a variable of the proper type e.g. "char x", then pass the
	address of your variable to the function by putting '&' in front of it,
	e.g. "readGain(&x)". The function will modify the variable directly.

	For information on the data sent to and received from the amplifier,
	refer to the TPA2016D2 datasheet at:
	http://www.ti.com/lit/ds/symlink/tpa2016d2.pdf
	
  License:
  We use the "beerware" license for our firmware. You can do
  ANYTHING you want with this code. If you like it, and we meet
  someday, you can, but are under no obligation to, buy me a
  (root) beer in return.

  Have fun! 
  -your friends at SparkFun

  Revision history:
  version 1.0 2012/07/24 MDG Initial release 
*/

#ifndef SFE_TPA2016D2_h
#define SFE_TPA2016D2_h

class SFE_TPA2016D2
{
	public:
	
		SFE_TPA2016D2();

		// Enable and disable speakers (mute)
		unsigned char enableSpeakers();
		unsigned char disableSpeakers();
		unsigned char enableRightSpeaker();
		unsigned char disableRightSpeaker();
		unsigned char enableLeftSpeaker();
		unsigned char disableLeftSpeaker();
		
		// Shut down the amplifier
		unsigned char enableShutdown();
		unsigned char disableShutdown();
		
		// Read fault conditions
		unsigned char readFaults(unsigned char *left, unsigned char *right, unsigned char *thermal);
		
		// Enable and disable noise gate
		unsigned char enableNoiseGate();
		unsigned char disableNoiseGate();
		
		// Read/write AGC attack time (0-63, see datasheet for units)
		unsigned char writeAttack(unsigned char attack);
		unsigned char readAttack(unsigned char *attack);
		
		// Read/write AGC release time (0-63, see datasheet for units)
		unsigned char writeRelease(unsigned char release);
		unsigned char readRelease(unsigned char *release);
		
		// Read/write AGC hold time (0-63, see datasheet for units)
		unsigned char writeHold(unsigned char hold);
		unsigned char readHold(unsigned char *hold);
		
		// Read/write AGC fixed gain (-28 to +30, see datasheet for units)
		unsigned char writeFixedGain(char gain);
		unsigned char readFixedGain(char *gain);
		
		// Enable and disable limiter
		unsigned char disableLimiter();
		unsigned char enableLimiter();
		
		// Read/write AGC noise gate threshold (0-3, see datasheet for units)
		unsigned char writeNoiseGateThreshold(unsigned char noisegatethreshold);
		unsigned char readNoiseGateThreshold(unsigned char *noisegatethreshold);
		
		// Read/write AGC output limiter level (0-31, see datasheet for units)
		unsigned char writeOutputLimiterLevel(unsigned char outputlimiterlevel);
		unsigned char readOutputLimiterLevel(unsigned char *outputlimiterlevel);

		// Read/write AGC max gain (0-15, see datasheet for units)
		unsigned char writeMaxGain(unsigned char maxgain);
		unsigned char readMaxGain(unsigned char *maxgain);

		// Read/write AGC compression ratio (0-3, see datasheet for units)
		unsigned char writeCompressionRatio(unsigned char compressionratio);
		unsigned char readCompressionRatio(unsigned char *compressionratio);

		// General-purpose register read/write
		unsigned char writeRegister(unsigned char reg, unsigned char value);
		unsigned char readRegister(unsigned char reg, unsigned char *value);

	private:

};

// I2C address (7-bit format for Wire library)
#define TPA2016D2_ADDR 0x58

// TPA2016D2 registers
#define TPA2016D2_CONTROL_REGISTER 1
#define TPA2016D2_ATTACK_REGISTER 2
#define TPA2016D2_RELEASE_REGISTER 3
#define TPA2016D2_HOLD_REGISTER 4
#define TPA2016D2_GAIN_REGISTER 5
#define TPA2016D2_AGC1_REGISTER 6
#define TPA2016D2_AGC2_REGISTER 7

#endif
