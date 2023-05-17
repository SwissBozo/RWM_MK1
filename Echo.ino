
boolean echoWrapped = false;

unsigned int echoDelay;
unsigned int readBuf[2];

volatile long address = 0;

void loopEcho(int pin) {
  echoDelay = analogRead(pin) * 30;


#ifdef DEBUG_ECHO
  if ((millis() - lastDebugPrint) >= 1000) {
    lastDebugPrint = millis();

    //each 3 echoDelay2 is 2 samples.
    unsigned int delayMillis = (float)(((float)((echoDelay * 2) / 3)) / (float)(sampleRate / 1000.0));

    Serial.print("echo delay = ");
    Serial.print(delayMillis);
    Serial.print(" ms    even cycles remaining = ");
    Serial.print(UINT16_MAX - timer1EndEven);
    Serial.print("   odd cycles remaining = ");
    Serial.print(UINT16_MAX - timer1EndOdd);
    Serial.println();
  }

#endif

}




void applyEcho(unsigned int &playbackBuf, unsigned int &signal) {


  unsigned int echo;
  int mix;

  // AudioHacker.writeDAC(playbackBuf);

  // Read ADC
  // signal = AudioHacker.readADC();


  if (evenCycle) {
    long echoAddress = address - echoDelay;
    if (echoAddress < 0) {
      echoAddress += MAX_ADDR;
    }
    AudioHacker.readSRAMPacked(0, echoAddress, readBuf);
    if ((!echoWrapped) && (echoAddress > address)) {
      // avoid reading from unwritten memory
      echo = 2048;
      readBuf[1] = 2048;
    } else {
      echo = readBuf[0];
    }
  } else {
    echo = readBuf[1];
  }
  if (echoDelay == 0) {
    echo = 2048;
  }

  if (evenCycle) {
    writeBuf = signal;
  } else {
    AudioHacker.writeSRAMPacked(0, address, writeBuf, signal);
    address += 3;
    if (address > MAX_ADDR) {
      address = 0;
      echoWrapped = true;
    }
  }


  mix = signal - 2048;
  echo = echo >> 1; // attenuate echo
  mix += (echo - 1024); // since we are dividing echo by 2, decrement by 1024
  if (mix < -2048) {
    mix = -2048;
  } else {
    if (mix > 2047) {
      mix = 2047;
    }
  }
  playbackBuf = mix + 2048;


}
