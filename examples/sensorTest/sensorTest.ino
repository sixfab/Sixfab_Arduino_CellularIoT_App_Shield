/*
  sensorTest.ino - This is basic sensor test example.
  Created by Yasin Kaya (selengalp), july 13, 2018.

  Warning : This example could be use with only Arduino CellularIoT App Shield.
*/

#include "Sixfab_CellularIoT.h"

SixfabCellularIoTApp node;

double ax, ay, az;

// setup
void setup() {
  
  node.init();
  
  node.turnOnUserLED();
  delay(TIMEOUT);
  node.turnOffUserLED();
  delay(TIMEOUT);
  node.turnOnRelay();
  delay(TIMEOUT);
  node.turnOffRelay();
  delay(TIMEOUT);

  node.readAccel(&ax, &ay, &az);
  DEBUG.print("ax: "); DEBUG.println(ax);
  DEBUG.print("ay: "); DEBUG.println(ay);
  DEBUG.print("az: "); DEBUG.println(az);
  
  DEBUG.println("Temperature: ");DEBUG.print(node.readTemp());DEBUG.println();
  delay(TIMEOUT);
  DEBUG.println("Humidity: ");DEBUG.print(node.readHum());DEBUG.println();
  delay(TIMEOUT);
  DEBUG.println("Light: ");DEBUG.print(node.readLux());
  delay(TIMEOUT);
}

// loop
void loop() {

}