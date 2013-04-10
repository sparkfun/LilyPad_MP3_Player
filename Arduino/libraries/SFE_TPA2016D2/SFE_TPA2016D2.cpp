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
	
	All functions return 1 if the read/write was successful, and 0
	if there was a communications failure. You can ignore the return value
	if you just don't care anymore.
	
	For functions that return a value, e.g. "readGain(char *gain)", first
	declare	a variable of the proper type e.g. "char x", then pass the
	address of your variable to the function by putting '&' in front of it
	e.g. "readGain(&x)". The function will modify the variable directly.

	For information on the data sent to and received from the amplifier,
	refer to the TPA2016D2 datasheet at:
	http://www.ti.com/lit/ds/symlink/tpa2016d2.pdf
	
	Included functions:

	unsigned char SFE_TPA2016D2::enableRightSpeaker()
	unsigned char SFE_TPA2016D2::disableRightSpeaker()
	
	unsigned char SFE_TPA2016D2::enableLeftSpeaker()
	unsigned char SFE_TPA2016D2::disableLeftSpeaker()
	
	unsigned char SFE_TPA2016D2::enableShutdown()
	unsigned char SFE_TPA2016D2::disableShutdown()
	
	unsigned char SFE_TPA2016D2::readFaults(unsigned char *left, unsigned char *right, unsigned char *thermal)
	
	unsigned char SFE_TPA2016D2::enableNoiseGate()
	unsigned char SFE_TPA2016D2::disableNoiseGate()
	
	unsigned char SFE_TPA2016D2::writeAttack(unsigned char attack)
	unsigned char SFE_TPA2016D2::readAttack(unsigned char *attack)
	
	unsigned char SFE_TPA2016D2::writeRelease(unsigned char release)
	unsigned char SFE_TPA2016D2::readRelease(unsigned char *release)
	
	unsigned char SFE_TPA2016D2::writeHold(unsigned char hold)
	unsigned char SFE_TPA2016D2::readHold(unsigned char *hold)
	
	unsigned char SFE_TPA2016D2::writeGain(char gain)
	unsigned char SFE_TPA2016D2::readGain(char *gain)
	
	unsigned char SFE_TPA2016D2::disableLimiter()
	unsigned char SFE_TPA2016D2::enableLimiter()
	
	unsigned char SFE_TPA2016D2::writeNoiseGateThreshold(unsigned char noisegatethreshold)
	unsigned char SFE_TPA2016D2::readNoiseGateThreshold(unsigned char *noisegatethreshold)
	
	unsigned char SFE_TPA2016D2::writeOutputLimiterLevel(unsigned char outputlimiterlevel)
	unsigned char SFE_TPA2016D2::readOutputLimiterLevel(unsigned char *outputlimiterlevel)
	
	unsigned char SFE_TPA2016D2::writeMaxGain(unsigned char maxgain)
	unsigned char SFE_TPA2016D2::readMaxGain(unsigned char *maxgain)
	
	unsigned char SFE_TPA2016D2::writeCompressionRatio(unsigned char compressionratio)
	unsigned char SFE_TPA2016D2::readCompressionRatio(unsigned char *compressionratio)

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

#include <SFE_TPA2016D2.h>
#include <Wire.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


SFE_TPA2016D2::SFE_TPA2016D2()
// Initialize the library (all we need to do is start up I2C)
{
	Wire.begin();
}


unsigned char SFE_TPA2016D2::writeRegister(unsigned char reg, unsigned char value)
// General-purpose write to a TPA2016D2 register
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char result;

  Wire.beginTransmission(TPA2016D2_ADDR);// I2C address (use 7-bit address, wire library will modify for read/write)
  Wire.write(reg);                       // register to write
  Wire.write(value);                     // value to write
  result = Wire.endTransmission();

  if (result == 0)
     return 1;
  return 0;
}


unsigned char SFE_TPA2016D2::readRegister(unsigned char reg, unsigned char *value)
// General-purpose read from a TPA2016D2 register
// Call with the address of your value variable e.g. &value
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char result, x;

  Wire.beginTransmission(TPA2016D2_ADDR);// i2c address (use 7-bit address, wire library will modify for read/write)
  Wire.write(reg);                       // register to read
  result = Wire.endTransmission();

  if (result == 0) // successful setup
  {
    Wire.requestFrom(TPA2016D2_ADDR, 1); 
    x = Wire.available(); // make sure read was successful
  
    if (x == 1)
    {
        *value = Wire.read();
        return 1;
    }
  }
  return 0;
}

unsigned char SFE_TPA2016D2::enableSpeakers()
// Enable both output channels (unmute)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_CONTROL_REGISTER,((regvalue | B11000000))));
      return 1;
  }
  return 0;
}

unsigned char SFE_TPA2016D2::disableSpeakers()
// Disable both output channels (mute)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_CONTROL_REGISTER,((regvalue & B00111111))));
      return 1;
  }
  return 0;
}

unsigned char SFE_TPA2016D2::enableRightSpeaker()
// Enable the right output channel
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_CONTROL_REGISTER,((regvalue | B10000000))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::disableRightSpeaker()
// Disable the right output channel
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_CONTROL_REGISTER,((regvalue & B01111111))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::enableLeftSpeaker()
// Enable the left output channel
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_CONTROL_REGISTER,((regvalue | B01000000))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::disableLeftSpeaker()
// Disable the left output channel
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_CONTROL_REGISTER,((regvalue & B10111111))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::enableShutdown()
// Shut down the amplifier (overrides external input)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_CONTROL_REGISTER,((regvalue | B00100000))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::disableShutdown()
// Enable the amplifier (external input will override this setting)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_CONTROL_REGISTER,((regvalue & B11011111))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::readFaults(unsigned char *left, unsigned char *right, unsigned char *thermal)
// Return all three fault settings.
// Call with addresses of your variables e.g. &left
// Your variable will be 1 if there is a fault, 0 otherwise.
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register and extract the bits we need
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    *right   = ((regvalue & B00010000) >> 4);	// mask and shift results
    *left    = ((regvalue & B00001000) >> 3);
    *thermal = ((regvalue & B00000100) >> 2);
    return 1;
  }
  else
    return 0;
}


unsigned char SFE_TPA2016D2::enableNoiseGate()
// Enable the noise gate feature
// Note that the noise gate can only be enabled if the compression ratio is not 1:1
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_CONTROL_REGISTER,((regvalue | B00000001))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::disableNoiseGate()
// Disable the noise gate feature
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_CONTROL_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_CONTROL_REGISTER,((regvalue & B11111110))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::writeAttack(unsigned char attack)
// Write the AGC attack time (0-63, see datasheet for units)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::writeRegister(TPA2016D2_ATTACK_REGISTER,(attack & B00111111)))
    return 1;
  else
    return 0;
}


unsigned char SFE_TPA2016D2::readAttack(unsigned char *attack)
// Read the AGC attack time (0-63, see datasheet for units)
// Call with address of your variable e.g. &attack
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::readRegister(TPA2016D2_ATTACK_REGISTER,attack))
    return 1;
  else
    return 0;
}


unsigned char SFE_TPA2016D2::writeRelease(unsigned char release)
// Write the AGC release time (0-63, see datasheet for units)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::writeRegister(TPA2016D2_RELEASE_REGISTER,(release & B00111111)))
    return 1;
  else
    return 0;
}


unsigned char SFE_TPA2016D2::readRelease(unsigned char *release)
// Read the AGC release time (0-63, see datasheet for units)
// Call with address of your variable e.g. &release
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::readRegister(TPA2016D2_RELEASE_REGISTER,release))
    return 1;
  else
    return 0;
}


unsigned char SFE_TPA2016D2::writeHold(unsigned char hold)
// Write the AGC hold time (0-63, see datasheet for units)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::writeRegister(TPA2016D2_HOLD_REGISTER,(hold & B00111111)))
    return 1;
  else
    return 0;
}


unsigned char SFE_TPA2016D2::readHold(unsigned char *hold)
// Read the AGC hold time (0-63, see datasheet for units)
// Call with address of your variable e.g. &hold
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::readRegister(TPA2016D2_HOLD_REGISTER,hold))
    return 1;
  else
    return 0;
}


unsigned char SFE_TPA2016D2::writeFixedGain(char gain)
// Write the AGC fixed gain (-28 to +30, see datasheet for units)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::writeRegister(TPA2016D2_GAIN_REGISTER,(gain & B00111111)))
    return 1;
  else
    return 0;
}


unsigned char SFE_TPA2016D2::readFixedGain(char *gain)
// Read the AGC fixed gain (-28 to +30, see datasheet for units)
// Call with address of your variable e.g. &gain
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

  if (SFE_TPA2016D2::readRegister(TPA2016D2_GAIN_REGISTER,&regvalue))
  {
    if (regvalue & B00100000) // negative number?
    {
      regvalue |= B11000000; // sign-extend, so that it stays negative
    }
    *gain = (char)regvalue;
    return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::disableLimiter()
// Set the compression ratio to 1:1 (required) and disable the output limiter
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the registers, modify only the bits we need, and write back the modified values
  if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC2_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_AGC2_REGISTER,(regvalue & B11111100))) // set compression ratio to 0
    {
      if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC1_REGISTER,&regvalue))
      {
        if (SFE_TPA2016D2::writeRegister(TPA2016D2_AGC1_REGISTER,(regvalue | B10000000))) // disable limiter
          return 1;
      }
    }
  }
  return 0;
}


unsigned char SFE_TPA2016D2::enableLimiter()
// Enable the output limiter
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC1_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_AGC1_REGISTER,(regvalue & B01111111))) // enable limiter
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::writeNoiseGateThreshold(unsigned char noisegatethreshold)
// Write AGC noise gate threshold (0-3, see datasheet for units)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC1_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_AGC1_REGISTER,((regvalue & B10011111) | ((noisegatethreshold << 5) & B01100000))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::readNoiseGateThreshold(unsigned char *noisegatethreshold)
// Read AGC noise gate threshold (0-3, see datasheet for units)
// Call with address of your variable e.g. &noisegatethreshold
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC1_REGISTER,noisegatethreshold))
  {
    *noisegatethreshold = ((*noisegatethreshold >> 5) & B00000011);	// shift and mask result
    return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::writeOutputLimiterLevel(unsigned char outputlimiterlevel)
// Write AGC output limiter level (0-31, see datasheet for units)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC1_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_AGC1_REGISTER,((regvalue & B11100000) | (outputlimiterlevel & B00011111))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::readOutputLimiterLevel(unsigned char *outputlimiterlevel)
// Read AGC output limiter level (0-31, see datasheet for units)
// Call with address of your variable e.g. &outputlimiterlevel
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC1_REGISTER,outputlimiterlevel))
  {
    *outputlimiterlevel &= B00011111;	// mask result
    return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::writeMaxGain(unsigned char maxgain)
// Write AGC max gain (0-15, see datasheet for units)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC2_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_AGC2_REGISTER,((regvalue & B00001111) | ((maxgain << 4) & B11110000))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::readMaxGain(unsigned char *maxgain)
// Read AGC max gain (0-15, see datasheet for units)
// Call with address of your variable e.g. &maxgain
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC2_REGISTER,maxgain))
  {
    *maxgain = ((*maxgain >> 4) & B00001111);	// shift and mask result
    return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::writeCompressionRatio(unsigned char compressionratio)
// Write AGC compression ratio (0-3, see datasheet for units)
// Returns 1 if successful, 0 if something failed (I2C error)
{
  unsigned char regvalue;

	// Read the register, modify only the bits we need, and write back the modified value
  if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC2_REGISTER,&regvalue))
  {
    if (SFE_TPA2016D2::writeRegister(TPA2016D2_AGC2_REGISTER,((regvalue & B11111100) | (compressionratio & B00000011))));
      return 1;
  }
  return 0;
}


unsigned char SFE_TPA2016D2::readCompressionRatio(unsigned char *compressionratio)
// Read AGC compression ratio (0-3, see datasheet for units)
// Call with address of your variable e.g. &compressionratio
// Returns 1 if successful, 0 if something failed (I2C error)
{
  if (SFE_TPA2016D2::readRegister(TPA2016D2_AGC2_REGISTER,compressionratio))
  {
    *compressionratio &= B00000011;	// mask result
    return 1;
  }
  return 0;
}

