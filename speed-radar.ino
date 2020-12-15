/* This is the code for a DIY speed sensor using the Doppler effect, and store the result on an SD card
// It is based on:
// the (German) speed radar tutorial by http://www.hjberndt.de/soft/radar/index.html, 
// Moritz Metz http://www.moritzmetz.de/ and his Netzbasteln Podcast https://www.deutschlandfunknova.de/beitrag/netzbasteln-radarkontrolle-diy 
// the JoTCon '19 Conference
// this tutorial on reading  / writing to SD card: https://create.arduino.cc/projecthub/electropeak/sd-card-module-with-arduino-how-to-read-write-data-37f390
// 
// Possible next steps: include RTC, Lorawan connection
*/ 

int sensorPin = A0, v,vmax; //select Arduino sensor input, variables v and vmax; 
int sensorValue = 0;        //store value from sensor
int threshold = 700;        //define threshold value when sensor starts measuring (5V is 1023)
int hour, minute, second;   // optional: initialize variables for writing time on SD card later 

#include <SPI.h> //SPI card library
#include <SD.h>  //SD card library

File myFile; //SD Card File

void setup() { 
  Serial.begin(9600);
  SD.begin(10); // SD input
  pinMode(sensorPin, INPUT);

/*
while (!Serial) {
; // wait for serial port to connect. Needed for native USB port only
}
Serial.print("Initializing SD card...");
if (!SD.begin(10)) {
Serial.println("initialization failed!");
while (1);
}
Serial.println("initialization done.");
*/

myFile = SD.open("speed.txt", FILE_WRITE);    //Create & Open Speed.txt on SD card
  if (myFile) {                               // if successfully created
    myFile.println("Start");                  // write first line
    myFile.close();                           // close the file         
  }   
  else {
    // if the file didn't open, print an error:
    // Serial.println("error opening speed.txt");
  }
}

void loop() {
  unsigned long T;                      // period duration in s
  double f;                             // Frequency in MHz 
  char s[20];                           // output string
  vmax=0;                               // var to store max speed
  sensorValue = analogRead(sensorPin);  // Read analogue value of radar sensor
  
  if (sensorValue > threshold){         // if above threshold 
    while(digitalRead(sensorPin));      // 
    while(!digitalRead(sensorPin));     // 
      for(int i=0;i<16;i++)             // 16 measurements
        {
          T = pulseIn(sensorPin, HIGH) + pulseIn(sensorPin, LOW);  // sensor input
          f=1/(double)T;           // f=1/T = frequency
          v=int((f*1e6)/44.0);     // 24 GHz radar
          vmax=max(v,vmax);        // compare v and vmax and write higher value in vmax 
          Serial.print(".");     // write some dots in serial monitor 
        } 
        
    second = (millis()/1000) % 60; 
    minute  = (millis()/60000) % 60;
    hour  = (millis()/3600000) % 24; // put together time since last start of program
    
    sprintf(s,"%02u:%02u:%02u,%d", hour, minute, second, vmax); // put together string s with time (since last start of prgramme) and vmax speed variable
    Serial.println(s);
    sdcardwrite(s);     // function sdcardwrite with argument s     
    delay(2000);        // Wait 2 seconds
}
  else {
  //nothing is moving
  }
} // end function loop()


void sdcardwrite(char speed[20]) {              // define function sdcardwrite with argument speed
    myFile = SD.open("speed.txt", FILE_WRITE);  // open file on SD card
    myFile.println(speed);                      // write string speed to file
    myFile.close();                             // close file
   // Serial.println("SD card write done");
}
