
// Include Libraries
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "Adafruit_GPS.h"
#include "NewPing.h"
#include "PiezoSpeaker.h"
#include "Button.h"


// Pin Definitions
#define FINGERPRINTSCANNER_5V_PIN_RX	17
#define FINGERPRINTSCANNER_5V_PIN_TX	16
#define GPSADAFUIT_PIN_RX	11
#define GPSADAFUIT_PIN_TX	10
#define HCSR04_PIN_TRIG	4
#define HCSR04_PIN_ECHO	3
#define PIEZOSPEAKER_5V_PIN_SIG	2
#define TILTSWITCH_PIN_2	5



// Global variables and defines
#define GPSECHO  true
uint32_t timer = millis(); //timer to read & print out the current stats
boolean usingInterrupt = false; // off by default! this keeps track of whether we're using the interrupt
unsigned int piezoSpeaker_5vHoorayLength          = 6;                                                      // amount of notes in melody
unsigned int piezoSpeaker_5vHoorayMelody[]        = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5}; // list of notes. List length must match HoorayLength!
unsigned int piezoSpeaker_5vHoorayNoteDurations[] = {8      , 8      , 8      , 4      , 8      , 4      }; // note durations; 4 = quarter note, 8 = eighth note, etc. List length must match HoorayLength!
// object initialization
HardwareSerial& bthc05(Serial1);
SoftwareSerial gpsSerial(GPSADAFUIT_PIN_TX,GPSADAFUIT_PIN_RX);
Adafruit_GPS gpsAdafuit(&gpsSerial);
NewPing hcsr04(HCSR04_PIN_TRIG,HCSR04_PIN_ECHO);
PiezoSpeaker piezoSpeaker_5v(PIEZOSPEAKER_5V_PIN_SIG);
Button TiltSwitch(TILTSWITCH_PIN_2);


// define vars for testing menu
const int timeout = 10000;       //define timeout of 10 sec
char menuOption = 0;
long time0;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() 
{
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial) ; // wait for serial port to connect. Needed for native USB
    Serial.println("start");
    
    bthc05.begin(9600);
    //This example uses HC-05 Bluetooth to communicate with an Android device.
    //Download bluetooth terminal from google play store, https://play.google.com/store/apps/details?id=Qwerty.BluetoothTerminal&hl=en
    //Pair and connect to 'HC-05', the default password for connection is '1234'.
    //You should see this message from your arduino on your android device
    bthc05.println("Bluetooth On....");
    gpsAdafuit.init();
    TiltSwitch.init();
    menuOption = menu();
    
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop() 
{
    
    
    if(menuOption == '1') {
    // HC - 05 Bluetooth Serial Module - Test Code
    String bthc05Str = "";
    //Receive String from bluetooth device
    if (bthc05.available())
    {
    //Read a complete line from bluetooth terminal
    bthc05Str = bthc05.readStringUntil('\n');
    // Print raw data to serial monitor
    Serial.print("BT Raw Data: ");
    Serial.println(bthc05Str);
    }
    //Send sensor data to Bluetooth device  
    bthc05.println("PUT YOUR SENSOR DATA HERE");
    }
    else if(menuOption == '2')
    {
    // Disclaimer: The Fingerprint Scanner - TTL (GT-511C3) is in testing and/or doesn't have code, therefore it may be buggy. Please be kind and report any bugs you may find.
    }
    else if(menuOption == '3') {
    // Adafruit Ultimate GPS Breakout - 66 channel w/10 Hz updates - Version 3 - Test Code
    if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = gpsAdafuit.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
    if (c) Serial.print(c);
    }
    // if a sentence is received, we can check the checksum, parse it...
    if (gpsAdafuit.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(gpsAdafuit.lastNMEA());   // this also sets the newNMEAreceived() flag to false
    if (!gpsAdafuit.parse(gpsAdafuit.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
    return;  // we can fail to parse a sentence in which case we should just wait for another
    }
    // if millis() or timer wraps around, we'll just reset it
    if (timer > millis())  timer = millis();
    // approximately every 2 seconds or so, print out the current stats
    if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    Serial.print(gpsAdafuit.hour, DEC); Serial.print(':');
    Serial.print(gpsAdafuit.minute, DEC); Serial.print(':');
    Serial.print(gpsAdafuit.seconds, DEC); Serial.print('.');
    Serial.println(gpsAdafuit.milliseconds);
    Serial.print("Date: ");
    Serial.print(gpsAdafuit.day, DEC); Serial.print('/');
    Serial.print(gpsAdafuit.month, DEC); Serial.print("/20");
    Serial.println(gpsAdafuit.year, DEC);
    Serial.print("Fix: "); Serial.print((int)gpsAdafuit.fix);
    Serial.print(" quality: "); Serial.println((int)gpsAdafuit.fixquality);
    if (gpsAdafuit.fix) {
    Serial.print("Location: ");
    Serial.print(gpsAdafuit.latitude, 4); Serial.print(gpsAdafuit.lat);
    Serial.print(", ");
    Serial.print(gpsAdafuit.longitude, 4); Serial.println(gpsAdafuit.lon);
    Serial.print("Location (in degrees, works with Google Maps): ");
    Serial.print(gpsAdafuit.latitudeDegrees, 4);
    Serial.print(", ");
    Serial.println(gpsAdafuit.longitudeDegrees, 4);
    Serial.print("Speed (knots): "); Serial.println(gpsAdafuit.speed);
    Serial.print("Angle: "); Serial.println(gpsAdafuit.angle);
    Serial.print("Altitude: "); Serial.println(gpsAdafuit.altitude);
    Serial.print("Satellites: "); Serial.println((int)gpsAdafuit.satellites);
    }
    }
    }
    else if(menuOption == '4') {
    // Ultrasonic Sensor - HC-SR04 - Test Code
    // Read distance measurment from UltraSonic sensor           
    int hcsr04Dist = hcsr04.ping_cm();
    delay(10);
    Serial.print(F("Distance: ")); Serial.print(hcsr04Dist); Serial.println(F("[cm]"));

    }
    else if(menuOption == '5') {
    // Piezo Speaker - Test Code
    // The Speaker will play the Hooray tune
    piezoSpeaker_5v.playMelody(piezoSpeaker_5vHoorayLength, piezoSpeaker_5vHoorayMelody, piezoSpeaker_5vHoorayNoteDurations); 
    delay(500);   
    }
    else if(menuOption == '6') {
    // Tilt Sensor - AT407 - Test Code
    //Read Tilt Switch state. 
    //if Switch is tilted function will return HIGH (1). if not function will return LOW (0). 
    //for debounce funtionality try also TiltSwitch.onPress(), .onRelease() and .onChange().
    //if debounce is not working properly try changing 'debounceDelay' variable in Button.h
    bool TiltSwitchVal = TiltSwitch.read();
    Serial.print(F("Val: ")); Serial.println(TiltSwitchVal);

    }
    
    if (millis() - time0 > timeout)
    {
        menuOption = menu();
    }
    
}



// Menu function for selecting the components to be tested
// Follow serial monitor for instrcutions
char menu()
{

    Serial.println(F("\nWhich component would you like to test?"));
    Serial.println(F("(1) HC - 05 Bluetooth Serial Module"));
    Serial.println(F("(2) Fingerprint Scanner - TTL (GT-511C3)"));
    Serial.println(F("(3) Adafruit Ultimate GPS Breakout - 66 channel w/10 Hz updates - Version 3"));
    Serial.println(F("(4) Ultrasonic Sensor - HC-SR04"));
    Serial.println(F("(5) Piezo Speaker"));
    Serial.println(F("(6) Tilt Sensor - AT407"));
    Serial.println(F("(menu) send anything else or press on board reset button\n"));
    while (!Serial.available());

    // Read data from serial monitor if received
    while (Serial.available()) 
    {
        char c = Serial.read();
        if (isAlphaNumeric(c)) 
        {   
            
            if(c == '1') 
    			Serial.println(F("Now Testing HC - 05 Bluetooth Serial Module"));
    		else if(c == '2') 
    			Serial.println(F("Now Testing Fingerprint Scanner - TTL (GT-511C3) - note that this component doesn't have a test code"));
    		else if(c == '3') 
    			Serial.println(F("Now Testing Adafruit Ultimate GPS Breakout - 66 channel w/10 Hz updates - Version 3"));
    		else if(c == '4') 
    			Serial.println(F("Now Testing Ultrasonic Sensor - HC-SR04"));
    		else if(c == '5') 
    			Serial.println(F("Now Testing Piezo Speaker"));
    		else if(c == '6') 
    			Serial.println(F("Now Testing Tilt Sensor - AT407"));
            else
            {
                Serial.println(F("illegal input!"));
                return 0;
            }
            time0 = millis();
            return c;
        }
    }
}

/*******************************************************

*    Circuito.io is an automatic generator of schematics and code for off
*    the shelf hardware combinations.

*    Copyright (C) 2016 Roboplan Technologies Ltd.

*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.

*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*    In addition, and without limitation, to the disclaimers of warranties 
*    stated above and in the GNU General Public License version 3 (or any 
*    later version), Roboplan Technologies Ltd. ("Roboplan") offers this 
*    program subject to the following warranty disclaimers and by using 
*    this program you acknowledge and agree to the following:
*    THIS PROGRAM IS PROVIDED ON AN "AS IS" AND "AS AVAILABLE" BASIS, AND 
*    WITHOUT WARRANTIES OF ANY KIND EITHER EXPRESS OR IMPLIED.  ROBOPLAN 
*    HEREBY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT 
*    NOT LIMITED TO IMPLIED WARRANTIES OF MERCHANTABILITY, TITLE, FITNESS 
*    FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT, AND THOSE ARISING BY 
*    STATUTE OR FROM A COURSE OF DEALING OR USAGE OF TRADE. 
*    YOUR RELIANCE ON, OR USE OF THIS PROGRAM IS AT YOUR SOLE RISK.
*    ROBOPLAN DOES NOT GUARANTEE THAT THE PROGRAM WILL BE FREE OF, OR NOT 
*    SUSCEPTIBLE TO, BUGS, SECURITY BREACHES, OR VIRUSES. ROBOPLAN DOES 
*    NOT WARRANT THAT YOUR USE OF THE PROGRAM, INCLUDING PURSUANT TO 
*    SCHEMATICS, INSTRUCTIONS OR RECOMMENDATIONS OF ROBOPLAN, WILL BE SAFE 
*    FOR PERSONAL USE OR FOR PRODUCTION OR COMMERCIAL USE, WILL NOT 
*    VIOLATE ANY THIRD PARTY RIGHTS, WILL PROVIDE THE INTENDED OR DESIRED
*    RESULTS, OR OPERATE AS YOU INTENDED OR AS MAY BE INDICATED BY ROBOPLAN. 
*    YOU HEREBY WAIVE, AGREE NOT TO ASSERT AGAINST, AND RELEASE ROBOPLAN, 
*    ITS LICENSORS AND AFFILIATES FROM, ANY CLAIMS IN CONNECTION WITH ANY OF 
*    THE ABOVE. 
********************************************************/