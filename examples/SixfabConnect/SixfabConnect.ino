/*
  localHost.ino - This is basic local host example.
  Created by Yasin Kaya (selengalp), August 18, 2018.
*/

#include "Sixfab_CellularIoT.h"

SixfabCellularIoT node;

char your_server[] = "XXXXXXXXXXXXXXXXXXXXXXXX"; // change with server url
char api_key[] = "XXXXXXXXXXXXXXXXXX"; // change with x-api-key
char device_id[] = "XXXXXXXXXXXXXXXXXXXXX"; //change with deviceId

char dataTemplate[] ="{\"deviceId\":\"%s\",\"temperature\":%d}";

char dataJSON[100];

// setup
void setup() {
  
  node.init();

  node.getIMEI();
  node.getFirmwareInfo();
  node.getHardwareInfo();

  node.setGSMBand(GSM_900);
  //node.setCATM1Band(LTE_B5);
  //node.setNBIoTBand(LTE_B20);
  node.getBandConfiguration();  
  node.setMode(GSM_MODE);
  
  node.connectToOperator();
  node.getSignalQuality();
  node.getQueryNetworkInfo();

  node.deactivateContext();
  node.activateContext();

  sprintf(dataJSON, dataTemplate, api_key, 20); 
  node.sendDataSixfabConnect(your_server, api_key, dataJSON );
  
}

// loop
void loop() {

}
