#include <USBHost_t36.h>

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb); 
MIDIDevice midi1(myusb);

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioPlaySdWav           playWav6; //xy=199,461
AudioPlaySdWav           playWav7; //xy=200,494
AudioPlaySdWav           playWav4; //xy=202,387
AudioPlaySdWav           playWav5; //xy=202,421
AudioPlaySdWav           playWav3; //xy=204,336
AudioPlaySdWav           playWav8; //xy=204,532
AudioPlaySdWav           playWav2; //xy=206,293
AudioPlaySdWav           playWav1; //xy=209,253
AudioMixer4              mixer4; //xy=407,510
AudioMixer4              mixer2; //xy=414,344
AudioMixer4              mixer3; //xy=414,419
AudioMixer4              mixer1; //xy=416,274
AudioMixer4              mixer5; //xy=630,386
AudioOutputI2S           i2s1;   //xy=797,402
AudioConnection          patchCord1(playWav6, 0, mixer3, 2);
AudioConnection          patchCord2(playWav6, 1, mixer3, 3);
AudioConnection          patchCord3(playWav7, 0, mixer4, 0);
AudioConnection          patchCord4(playWav7, 1, mixer4, 1);
AudioConnection          patchCord5(playWav4, 0, mixer2, 2);
AudioConnection          patchCord6(playWav4, 1, mixer2, 3);
AudioConnection          patchCord7(playWav5, 0, mixer3, 0);
AudioConnection          patchCord8(playWav5, 1, mixer3, 1);
AudioConnection          patchCord9(playWav3, 0, mixer2, 0);
AudioConnection          patchCord10(playWav3, 1, mixer2, 1);
AudioConnection          patchCord11(playWav8, 0, mixer4, 2);
AudioConnection          patchCord12(playWav8, 1, mixer4, 3);
AudioConnection          patchCord13(playWav2, 0, mixer1, 2);
AudioConnection          patchCord14(playWav2, 1, mixer1, 3);
AudioConnection          patchCord15(playWav1, 0, mixer1, 0);
AudioConnection          patchCord16(playWav1, 1, mixer1, 1);
AudioConnection          patchCord17(mixer4, 0, mixer5, 3);
AudioConnection          patchCord18(mixer2, 0, mixer5, 1);
AudioConnection          patchCord19(mixer3, 0, mixer5, 2);
AudioConnection          patchCord20(mixer1, 0, mixer5, 0);
AudioConnection          patchCord21(mixer5, 0, i2s1, 0);
AudioConnection          patchCord22(mixer5, 0, i2s1, 1);

/*
AudioPlaySdWav           playWav1;
// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
AudioOutputI2S           audioOutput;
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);*/
AudioControlSGTL5000     sgtl5000_1;

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14


//choosing int array so we can store probability potentially in the future
byte steps [8][8];
byte stepclock = 0;
byte currentstep = 1;
byte lastread = 0;
bool stepdone = false;
bool stepchance = false;

void setup() {
  Serial.begin(115200);

  // Wait 1.5 seconds before turning on USB Host.  If connected USB devices
  // use too much power, Teensy at least completes USB enumeration, which
  // makes isolating the power issue easier.
  delay(1500);
  //Serial.println("USB Host InputFunctions example");
  delay(10);
  myusb.begin();
  midi1.setHandleNoteOn(myNoteOn);
  midi1.setHandleNoteOff(myNoteOff);
  midi1.setHandleControlChange(myControlChange);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      //Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

    
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
}

void loop() {
  stepclock = stepclock+1;
  byte current;
  current = digitalRead(40);
  Serial.println(current);
  //if (stepclock == 50) {

  
  if (current != lastread && lastread == 0) {
    for (int y=0; y<8; y++) {
     if (steps[currentstep][y] == 1) {
       pickclip(y);    
     } else if (steps[currentstep][y] == 2) {
       if (stepchance == true) {
        pickclip(y);
       }
     }
    }

    //Serial.println(stepchance);

    if (currentstep == 7) {
      currentstep = 0;
      if (stepchance == true) {
        stepchance = false;
      } else if (stepchance == false){
        stepchance = true;
      }
    } else {
      currentstep = currentstep + 1;
    }
    
    //stepclock = 0;
    
  }
   
  myusb.Task();
  midi1.read();
  for (int x=0; x<=7; x++) {
    for (int y=0; y<=7; y++) {
      int note = x + (y*10 + 10) +1;
      if (steps[x][y] != 0) {
        midi1.sendNoteOn(note, steps[x][y]*20, 1);
      } else {
        midi1.sendNoteOn(note, 0, 1);
      }
    }
  }
  
  lastread = current;
}

/*
    Each row runs from x1 to x8, with the arrow key on x9.
    ex: 11 to 18 are the pads, with 19 as the arrow key
*/

void pickclip(byte inst){
  if (inst == 0) {
    playWav1.play("KICK.WAV"); 
  } else if (inst == 1) {
    playWav2.play("SNARE.WAV");  
  } else if (inst == 2) {
    playWav3.play("OHAT.WAV");  
  } else if (inst == 3) {
    playWav4.play("CHAT.WAV");  
  } else if (inst == 4) {
    playWav5.play("KICK.WAV");  
  } else if (inst == 5) {
    playWav6.play("HTOM.WAV"); 
  } else if (inst == 6) {
    playWav7.play("MTOM.WAV");    
  } else if (inst == 7) {
    playWav8.play("LTOM.WAV");    
  }
}

void myNoteOn(byte channel, byte note, byte velocity) {
  // When a USB device with multiple virtual cables is used,
  // midi1.getCable() can be used to read which of the virtual
  // MIDI cables received this message.
  /*Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);*/

  //breaking these conversion statements somehow crashes the serial output, not sure how, it just does
  int x = note - ((floor(note/10)) * 10) - 1;
  int y = ((floor(note/10)) * 10 - 10)/10;
  
  //Serial.println(x);
  //Serial.println(y);
  
  if (steps[x][y] == 0){
   steps[x][y] = 1;   
  } else if (steps[x][y] == 1){
    steps[x][y] = 2;    
  } else {
    steps[x][y] = 0;  
  }
  
  //Serial.println((steps[x][y]));
}

void playFile(const char *filename)
{
  //Serial.print("Playing file: ");
  //Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  //delay(25);

  // Simply wait for the file to finish playing.
  //while (playWav1.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);
  //}
}


void myNoteOff(byte channel, byte note, byte velocity) {
  /*Serial.print("Note Off, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);*/
}

void myControlChange(byte channel, byte control, byte value) {
  /*Serial.print("Control Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", control=");
  Serial.print(control, DEC);
  Serial.print(", value=");
  Serial.println(value, DEC);*/
}
