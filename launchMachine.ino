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


AudioPlaySdWav           playWav1;
// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
AudioOutputI2S           audioOutput;
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14


//choosing int array so we can store probability potentially in the future
int steps [8][8];

int stepclock = 0;
int currentstep = 1;
bool stepdone = false;


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
  if (stepclock == 20) {
    
    for (int y=0; y<8; y++) {
     if (steps[currentstep][y] == 1) {
       playFile("KICK.WAV");
     }
    }

    Serial.println(currentstep);

    if (currentstep == 7) {
      currentstep = 0;
    } else {
      currentstep = currentstep + 1;
    }
    
    stepclock = 0;
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
  
}

/*
    Each row runs from x1 to x8, with the arrow key on x9.
    ex: 11 to 18 are the pads, with 19 as the arrow key
*/



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
