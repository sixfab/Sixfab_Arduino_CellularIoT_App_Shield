/*
  localHost.ino - This is basic local host example.
  Created by Yasin Kaya (selengalp), August 18, 2018.
*/

#include "Sixfab_CellularIoT.h"

SixfabCellularIoT node;

char your_ip[] = "xx.xx.xx.xx"; // change with your ip
char your_port[] = "xxxx"; // change with your port

// sensor variables
int temp = 0;
int hum = 0;
int light = 0;

char data_str[200];

// setup
void setup() {
  
  node.init();

  node.getIMEI();
  node.getFirmwareInfo();
  node.getHardwareInfo();

  node.setIPAddress(your_ip);
  node.setPort(your_port);

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
  node.closeConnection();
  node.startUDPService();
  
  node.sendDataUDP("Hello World!\r\n");
}

// loop
void loop() {
  // read sensors
  temp = node.readTemp();
  hum = node.readHum();
  light = node.readLux();

  sprintf(data_str, "Temperature: %d  -  Humidity: %d  -  Light: %d\r\n", temp, hum, light);
  
  DEBUG.print(data_str);

  node.sendDataUDP(data_str); // send sensor data to server 

  delay(5000);
}