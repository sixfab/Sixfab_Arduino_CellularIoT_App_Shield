/*
  parseGNSS.ino - GNSS parse example.
  Created by Saeed Johar (saeedjohar), August 7, 2019.
*/

#include "Sixfab_CellularIoT.h"

SixfabCellularIoTApp node; // for Cellular IoT Application Shield
//SixfabCellularIoT node; // uncomment for Cellular IoT Shield

String gpsSentence; // To store GPS data/sentence
int pos = 0;
void setup() {
  
  node.init(); // initialize the module
  
  node.sendATComm("AT+QGPSCFG=\"outport\",\"uartnmea\"","OK\r\n");
  delay(500); // waits for 5ms
  node.turnOnGNSS(); //turns on the GNSS 
  
}

void loop() {
  
  gpsSentence = GNSS.readStringUntil(13); // ascii(13) is the 'return' character
  gpsSentence.trim();  //any leading and trailing whitespace removed
  //DEBUG.println(gpsSentence);
    
  delay(2);
// Starts parsing if RMC sentence received
    if (gpsSentence.startsWith("$GPRMC") || gpsSentence.startsWith("$GNRMC") ) {  
        
     //DEBUG.print("Raw RMC Data: ");DEBUG.println(gpsSentence); // displays raw RMC sentence
     returnData();  // returns $GPRMC or $GNRMC
     removeString();  //removes $GPRMC or $GNRMC

//UTC
    float utc = returnData().toFloat();//print(utc);
    removeString();    // removes UTC

//Validity
    String validity = returnData();
    removeString();    //removes validity
    
//Latitude
    float lat = returnData().toFloat();  
    removeString();  //remove latitude from the string

//North/South
    String northOrSouth = returnData();
    removeString();
  
//Longitude
    
    float lon = returnData().toFloat();
    removeString();     //remove latitude from the string

//East/West
    String eastOrWest = returnData();
    removeString();

  //DEBUG.print(validity);
    if ( validity == "A"){
      DEBUG.print("UTC Time "); DEBUG.print(utc);
      DEBUG.print("\tLat : ");DEBUG.print(ConvertData(lat),6);DEBUG.print("  ");DEBUG.print(northOrSouth);
      DEBUG.print("\tLon : ");DEBUG.print(ConvertData(lon),6);DEBUG.print("  ");DEBUG.println(eastOrWest);
    }
  }
  
}

// Function to read the string till comma in the GPS sentence
String returnData(){
  pos = gpsSentence.indexOf(','); // return index of first comma (,)
  return gpsSentence.substring(0,pos); // returns the String from 0 index till comma
  }

// Function to remove the read sub-string
void removeString(){
  pos = gpsSentence.indexOf(',');
  gpsSentence.remove(0, pos+1);
  }

//Function to convert the raw data to degree
float ConvertData(float RawDegrees){ 
  float RawAsFloat = RawDegrees; 
  int firstdigits = ((int)RawAsFloat)/100; // Get the first digits by turning f into an integer, then doing an integer divide by 100;
  float nexttwodigits = RawAsFloat - (float)(firstdigits*100);
  float Converted = (float)(firstdigits + nexttwodigits/60.0);
  return Converted;
}
