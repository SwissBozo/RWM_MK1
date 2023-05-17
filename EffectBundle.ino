boolean evenCycle = true;

/*
  Audio Hacker Library
  Copyright (C) 2013 nootropic design, LLC
  All rights reserved.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
  Echo Effect for Audio Hacker.

  See Audio Hacker project page for details.
  http://nootropicdesign.com/audiohacker/projects.html
*/

#include <AudioHacker.h>

#define DEBUG
#define DEBUG_ECHO
#define DEBUG_CRUSHRATE

unsigned int playbackBuf = 2048;
unsigned int sampleRate  = 22050;
unsigned int writeBuf;
unsigned int timer1Start;
unsigned int mask;

volatile unsigned int timer1EndEven;
volatile unsigned int timer1EndOdd;

unsigned long lastDebugPrint = 0;



void setup() {
#ifdef DEBUG
  Serial.begin(115200);        // connect to the serial port
#endif

  playbackBuf = 2048;
  sampleRate = DEFAULT_RECORDING_SAMPLE_RATE;
  timer1Start = UINT16_MAX - (F_CPU / sampleRate);

  AudioHacker.begin();

#ifdef DEBUG
  Serial.print("sample rate = ");
  Serial.print(sampleRate);
  Serial.print(" Hz");
  Serial.println();
#endif
}

void loop() {

  delay(300);
  // echoDelay is number of memory slots back into the past to read for the echo.
  // must be a factor of 3 because we pack 2 samples into each 3-byte sequence of SRAM.
  loopEcho(0);
  loopCrushRate(1);

}

ISR(TIMER1_OVF_vect) {
  TCNT1 = timer1Start;
  AudioHacker.writeDAC(playbackBuf);

  // ECHO
  unsigned int signal = AudioHacker.readADC();
  applyEcho(playbackBuf, signal);

  // SAMPLE & RATE
  applyCrushRate(mask);

#ifdef DEBUG_ECHO
  if (evenCycle) {
    timer1EndEven = TCNT1;
  } else {
    timer1EndOdd = TCNT1;
  }
#endif
  evenCycle = !evenCycle;
}
