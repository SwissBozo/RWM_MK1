//unsigned int mask;
//unsigned int timer1Start;
byte resolution = 12;
unsigned int CrushRateDelay;
//unsigned int readBuf[2];

void loopCrushRate(int pin) {

CrushRateDelay = map(analogRead(1), 0, 1023, 1000, 44100);
  
#ifdef DEBUG_CRUSHRATE
  if ((millis() - lastDebugPrint) >= 500) {
    lastDebugPrint = millis();

    Serial.print("sample rate = ");
    Serial.print(sampleRate);
    Serial.print("   resolution = ");
    Serial.print(resolution);
    Serial.print(" bit");
    if (resolution > 1) Serial.print("s");
    Serial.println();
  }
#endif

  delay(300);

}

void applyCrushRate(unsigned int &mask) {

  playbackBuf = AudioHacker.readADC();
  playbackBuf &= mask;
}
