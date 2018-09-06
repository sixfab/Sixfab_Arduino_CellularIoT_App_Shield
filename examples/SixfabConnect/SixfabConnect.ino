/*
  localHost.ino - This is basic local host example.
  Created by Yasin Kaya (selengalp), August 18, 2018.
*/

#include "Sixfab_CellularIoT.h"

SixfabCellularIoT node;

char your_server[] = "w8w35wd172.execute-api.us-east-1.amazonaws.com"; // change with server url
char api_key[] = "MqS7qFe6Az45xbzSZCMC03jK2vLEbHAN6Mfs0BSM"; // change with x-api-key
char device_id[] = "9f2e8999-1f77-4d27-9a80-4ff962e71ab3"; //change with deviceId

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
  //node.closeConnection();
  //node.connectToServerTCP();


  node.sendDataSixfabConnect(your_server, api_key, "{\"deviceId\":\"9f2e8999-1f77-4d27-9a80-4ff962e71ab3\",\"temperature\":20}" );
  sprintf(dataJSON, dataTemplate, api_key, 20); 
  
}

// loop
void loop() {

}
