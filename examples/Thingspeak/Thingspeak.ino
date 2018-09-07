/*
  localHost.ino - This is basic local host example.
  Created by Yasin Kaya (selengalp), August 18, 2018.
*/

#include "Sixfab_CellularIoT.h"

SixfabCellularIoT node;

char api_key[] = "XXXXXXXXXXXXXXX"; // change with api-key

char dataTemplate[] ="field1=%d";

char dataJSON[100];

// setup
void setup() {
  
  node.init();

  node.getIMEI();
  node.getFirmwareInfo();
  node.getHardwareInfo();

  node.setGSMBand(GSM_900);
  node.getBandConfiguration();  
  node.setMode(GSM_MODE);
  
  node.connectToOperator();
  node.getSignalQuality();
  node.getQueryNetworkInfo();

  node.deactivateContext();
  node.activateContext();

  sprintf(dataJSON, dataTemplate, 22);
  node.sendDataThingspeak(api_key, dataJSON );
  
}

// loop
void loop() {

}
