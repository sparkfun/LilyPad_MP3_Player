/* 
  SFE TPA2015D2 stereo class-D amplifier library
  Mike Grusin
  http://www.sparkfun.com

  This library provides a set of functions to control the TI TPA2016D2
  stereo amplifier via the I2C ("Wire") interface.

  HARDWARE CONNECTIONS

  Connect the I2C pins (SCL and SDA) from your Arduino to the same pins
  on the TPA2016D2. Check the documentation for your particular Arduino
  to find out which pins to use: http://arduino.cc/en/Reference/Wire

  Ensure that pullup resistors are present and enabled on the I2C lines.
  All Sparkfun boards which use the TPA2016D2 provide these resistors.

  Power the amplifier with 5V from your Arduino (3.3V on 3.3V Arduinos).
  Note that the amplifier can draw several hundred milliamps at full volume.
  
  EXAMPLE CODE
  
  The TPA2016D2 amplifier chip has a number of audio-processing settings
  that can be controled via I2C. This library provides access to these
  settings. To learn how to control the chip refer to the datasheet at:

  http://www.ti.com/lit/ds/symlink/tpa2016d2.pdf

  This example code will write random values to each of the TPA2016D2's
  registers, and read those values back, to illustrate the functions
  available in the library.
  
  For more information on the individual functions, refer to the comments
  in the library files: SFE_TPA2016D2.h and SFE_TPA2016D2.cpp.

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
  
  For all functions that return a value, e.g. "readGain(char *gain)",
  first declare a variable of the proper type e.g. "char x", then pass
  the address of your variable to the function by putting '&' in front
  of it, e.g. "readGain(&x)". The function will modify the variable directly.

  LICENSE:
  
  We use the "beerware" license for our firmware. You can do
  ANYTHING you want with this code. If you like it, and we meet
  someday, you can, but are under no obligation to, buy me a
  (root) beer in return.

  Have fun! 
  -your friends at SparkFun

  Revision history:
  version 1.0 2012/12/06 MDG Initial release 
*/

#include <Wire.h> 
#include <SFE_TPA2016D2.h>

SFE_TPA2016D2 amp;

void setup() 
{ 
  Serial.begin(9600);  // set up serial port
  Serial.println("hello!");
  
  // "seed" the random number generator
  randomSeed(analogRead(0)+analogRead(1));
  
  // display the default register values
  printAllRegisters();
  Serial.println();

  // The following code exercises each function in the library.
  
  // For functions that read data, the value will be read and displayed.
  // For functions that write data, a random value will be written to
  // that register, and read back and displayed. (At the end of the
  // code, the default values will be written back to the amplifier).

  Serial.println("GENERAL ENABLES"); Serial.println();

  Serial.print("disableRightSpeaker... ");
  if (amp.disableRightSpeaker()) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_CONTROL_REGISTER); Serial.println();
  
  Serial.print("enableRightSpeaker... ");
  if (amp.enableRightSpeaker()) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_CONTROL_REGISTER); Serial.println();
  
  Serial.print("disableLeftSpeaker... ");
  if (amp.disableLeftSpeaker()) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_CONTROL_REGISTER); Serial.println();

  Serial.print("enableLeftSpeaker... ");
  if (amp.enableLeftSpeaker()) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_CONTROL_REGISTER); Serial.println();

  Serial.print("enableShutdown... ");
  if (amp.enableShutdown()) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_CONTROL_REGISTER); Serial.println();

  Serial.print("disableShutdown... ");
  if (amp.disableShutdown()) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_CONTROL_REGISTER); Serial.println();

  Serial.println("READ FAULT FLAGS"); Serial.println();

  boolean right, left, thermal;

  Serial.print("readFaults... ");
  if (amp.readFaults(&left, &right, &thermal)) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_CONTROL_REGISTER);
  Serial.print("left: "); if (left) Serial.println("high"); else Serial.println("low");
  Serial.print("right: "); if (right) Serial.println("high"); else Serial.println("low");
  Serial.print("thermal: "); if (thermal) Serial.println("high"); else Serial.println("low");
  Serial.println();

  Serial.println("ATTACK TIME"); Serial.println();

  unsigned char attack;

  Serial.print("readAttack... ");
  if (amp.readAttack(&attack)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("attack: 0x"); Serial.println(attack,HEX);
  printRegister(TPA2016D2_ATTACK_REGISTER); Serial.println();

  attack = random(0x3F);
  Serial.print("writeAttack to 0x"); Serial.print(attack,HEX); Serial.print("... "); 
  if (amp.writeAttack(attack)) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_ATTACK_REGISTER); Serial.println();

  Serial.print("readAttack... ");
  if (amp.readAttack(&attack)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("attack: 0x"); Serial.println(attack,HEX);
  printRegister(TPA2016D2_ATTACK_REGISTER); Serial.println();

  Serial.println("RELEASE TIME"); Serial.println();

  unsigned char release;

  Serial.print("readRelease... ");
  if (amp.readRelease(&release)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("release: 0x"); Serial.println(release,HEX);
  printRegister(TPA2016D2_RELEASE_REGISTER); Serial.println();

  release = random(0x3F);
  Serial.print("writeRelease to 0x"); Serial.print(release,HEX); Serial.print("... "); 
  if (amp.writeRelease(release)) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_RELEASE_REGISTER); Serial.println();

  Serial.print("readRelease... ");
  if (amp.readRelease(&release)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("release: 0x"); Serial.println(release,HEX);
  printRegister(TPA2016D2_RELEASE_REGISTER); Serial.println();

  Serial.println("HOLD TIME"); Serial.println();

  unsigned char hold;

  Serial.print("readHold... ");
  if (amp.readHold(&hold)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("hold: 0x"); Serial.println(hold,HEX);
  printRegister(TPA2016D2_HOLD_REGISTER); Serial.println();

  hold = random(0x3F);
  Serial.print("writeHold to 0x"); Serial.print(hold,HEX); Serial.print("... "); 
  if (amp.writeHold(hold)) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_HOLD_REGISTER); Serial.println();

  Serial.print("readHold... ");
  if (amp.readHold(&hold)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("hold: 0x"); Serial.println(hold,HEX);
  printRegister(TPA2016D2_HOLD_REGISTER); Serial.println();

  Serial.println("GAIN"); Serial.println();

  char gain;

  Serial.print("readGain... ");
  if (amp.readGain(&gain)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("gain: "); Serial.print(gain,DEC); Serial.println(" (DEC)");
  printRegister(TPA2016D2_GAIN_REGISTER); Serial.println();

  gain = random(64)-32;
  Serial.print("writeGain to "); Serial.print(gain,DEC); Serial.print(" (DEC)... "); 
  if (amp.writeGain(gain)) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_GAIN_REGISTER); Serial.println();

  Serial.print("readGain... ");
  if (amp.readGain(&gain)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("gain: "); Serial.print(gain,DEC); Serial.println(" (DEC)");
  printRegister(TPA2016D2_GAIN_REGISTER); Serial.println();

  Serial.println("NOISE GATE"); Serial.println();

  Serial.print("disableNoiseGate... ");
  if (amp.disableNoiseGate()) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_CONTROL_REGISTER); Serial.println();

  Serial.print("enableNoiseGate... ");
  if (amp.enableNoiseGate()) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_CONTROL_REGISTER); Serial.println();

  unsigned char noisegatethreshold;
  
  Serial.print("readNoiseGateThreshold... ");
  if (amp.readNoiseGateThreshold(&noisegatethreshold)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("noisegatethreshold: 0x"); Serial.println(noisegatethreshold,HEX);
  printRegister(TPA2016D2_AGC1_REGISTER); Serial.println();

  noisegatethreshold = random(4);
  Serial.print("writeNoiseGateThreshold to 0x"); Serial.print(noisegatethreshold,HEX); Serial.print("... "); 
  if (amp.writeNoiseGateThreshold(noisegatethreshold)) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_AGC1_REGISTER); Serial.println();

  Serial.print("readNoiseGateThreshold... ");
  if (amp.readNoiseGateThreshold(&noisegatethreshold)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("noisegatethreshold: 0x"); Serial.println(noisegatethreshold,HEX);
  printRegister(TPA2016D2_AGC1_REGISTER); Serial.println();

  Serial.println("OUTPUT LIMITER"); Serial.println();

  Serial.print("disableLimiter... ");
  if (amp.disableLimiter()) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_AGC1_REGISTER);
  printRegister(TPA2016D2_AGC2_REGISTER); Serial.println();
  
  Serial.print("enableLimiter... ");
  if (amp.enableLimiter()) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_AGC1_REGISTER);
  printRegister(TPA2016D2_AGC2_REGISTER); Serial.println();
  
  unsigned char outputlimiterlevel;
  
  Serial.print("readOutputLimiterLevel... ");
  if (amp.readOutputLimiterLevel(&outputlimiterlevel)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("outputlimiterlevel: 0x"); Serial.println(outputlimiterlevel,HEX);
  printRegister(TPA2016D2_AGC1_REGISTER); Serial.println();

  outputlimiterlevel = random(32);
  Serial.print("writeOutputLimiterLevel to 0x"); Serial.print(outputlimiterlevel,HEX); Serial.print("... "); 
  if (amp.writeOutputLimiterLevel(outputlimiterlevel)) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_AGC1_REGISTER); Serial.println();

  Serial.print("readOutputLimiterLevel... ");
  if (amp.readOutputLimiterLevel(&outputlimiterlevel)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("outputlimiterlevel: 0x"); Serial.println(outputlimiterlevel,HEX);
  printRegister(TPA2016D2_AGC1_REGISTER); Serial.println();

  Serial.println("MAX GAIN"); Serial.println();
  
  unsigned char maxgain;
  
  Serial.print("readMaxGain... ");
  if (amp.readMaxGain(&maxgain)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("maxgain: 0x"); Serial.println(maxgain,HEX);
  printRegister(TPA2016D2_AGC2_REGISTER); Serial.println();

  maxgain = random(16);
  Serial.print("writeMaxGain to 0x"); Serial.print(maxgain,HEX); Serial.print("... "); 
  if (amp.writeMaxGain(maxgain)) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_AGC2_REGISTER); Serial.println();

  Serial.print("readMaxGain... ");
  if (amp.readMaxGain(&maxgain)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("maxgain: 0x"); Serial.println(maxgain,HEX);
  printRegister(TPA2016D2_AGC2_REGISTER); Serial.println();

  Serial.println("COMPRESSION RATIO"); Serial.println();
  
  unsigned char compressionratio;
  
  Serial.print("readCompressionRatio... ");
  if (amp.readCompressionRatio(&compressionratio)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("compressionratio: 0x"); Serial.println(compressionratio,HEX);
  printRegister(TPA2016D2_AGC2_REGISTER); Serial.println();

  compressionratio = random(4);
  Serial.print("writeCompressionRatio to 0x"); Serial.print(compressionratio,HEX); Serial.print("... "); 
  if (amp.writeCompressionRatio(compressionratio)) Serial.println("success!"); else Serial.println("FAIL");
  printRegister(TPA2016D2_AGC2_REGISTER); Serial.println();

  Serial.print("readCompressionRatio... ");
  if (amp.readCompressionRatio(&compressionratio)) Serial.println("success!"); else Serial.println("FAIL");
  Serial.print("compressionratio: 0x"); Serial.println(compressionratio,HEX);
  printRegister(TPA2016D2_AGC2_REGISTER); Serial.println();

  Serial.print("setting values back to defaults... ");
  amp.writeRegister(TPA2016D2_CONTROL_REGISTER, 0xC3);
  amp.writeRegister(TPA2016D2_ATTACK_REGISTER, 0x05);
  amp.writeRegister(TPA2016D2_RELEASE_REGISTER, 0x0B);
  amp.writeRegister(TPA2016D2_HOLD_REGISTER, 0x00);
  amp.writeRegister(TPA2016D2_GAIN_REGISTER, 0x06);
  amp.writeRegister(TPA2016D2_AGC1_REGISTER, 0x3A);
  amp.writeRegister(TPA2016D2_AGC2_REGISTER, 0xC2);
  Serial.println("done!");
} 

void loop()
{ 
  // we did all the tests once in setup, so do nothing here
}

void printAllRegisters()
// read and print out the values of the TPA2016D2 registers
{
  unsigned char reg, val;

  Serial.println("all registers:");
  for (reg = 1; reg <= 7; reg++)
    printRegister(reg);
}

void printRegister(unsigned char reg)
// Print out an individual TPA2016D2 register
{
  unsigned char val;

  if (amp.readRegister(reg,&val))
  {
    Serial.print("reg ");
    
    Serial.print(reg);
    Serial.print(": 0x");
    Serial.print(val,HEX);
    Serial.print(" B");
    printBinary(val);
    Serial.println();
  }
  else
    Serial.println("I2C error");
}

void printBinary(byte x)
// Utility routine to print a number (8 bits) in binary
{
  char y;
  
  // Step through all eight bits, MSB to LSB
  for (y = 7; y >= 0; y--)
  {
    // Print a space between the upper and lower nybbles
    if (y == 3) Serial.print(" ");
    
    // Check if the bit is a 1 or 0 and print that out
    if (x & (1 << y)) Serial.print("1"); else Serial.print("0");
  }
}

