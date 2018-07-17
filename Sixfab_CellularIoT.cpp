/*
  Sixfab_CellularIoT.cpp 
  -
  Library for Sixfab Arduino CellularIoT Shield and 
  Sixfab Arduino CellularIoT Application Shield.
  -
  Created by Yasin Kaya (selengalp), july 13, 2018.
*/

#include "Sixfab_CellularIoT.h"

Sixfab_HDC1080 hdc1080;
MMA8452Q accel; // I2C Address : 0x1C
SoftwareSerial BG96_AT(10,9); // RX, TX - 9600 baud rate 

// default
SixfabCellularIoT::SixfabCellularIoT()
{

}

/******************************************************************************************
 *** Base Functions : Set or Clear Hardwares - Status Controls - Helper AT Functions  *****
 ******************************************************************************************/

// function for initializing BG96 module.
void SixfabCellularIoT::init()
{
  // setting pin directions
  pinMode(USER_LED, OUTPUT);
  //pinMode(RELAY, OUTPUT);
  pinMode(USER_BUTTON, INPUT);
  //pinMode(ALS_PT19_PIN, INPUT);
  
  enable();

  powerUp();

  // setting serials
  BG96_AT.begin(9600);
  DEBUG.begin(115200);
  GNSS.begin(115200);
  
  DEBUG.println("Module initializing");

  // HDC1080 begin
  hdc1080.begin(0x40);
  // mma8452q init 
  accel.init();

  sendATComm("ATE1","OK\r\n"); 
  sendATComm("ATE1","OK\r\n");
  sendATComm("AT","OK\r\n");
}

// power up BG96 module and all peripherals from voltage regulator 
void SixfabCellularIoT::enable()
{
  pinMode(BG96_ENABLE, OUTPUT);
  digitalWrite(BG96_ENABLE,HIGH);
}

// power down BG96 module and all peripherals from voltage regulator 
void SixfabCellularIoT::disable()
{
  digitalWrite(BG96_ENABLE,LOW);
}

// power up or down BG96 module
void SixfabCellularIoT::powerUp()
{
  pinMode(BG96_POWERKEY,OUTPUT);
  delay(10);
  digitalWrite(BG96_POWERKEY,HIGH);
  delay(500);
  digitalWrite(BG96_POWERKEY,LOW);
  delay(6000);
}


// send at comamand to module
void SixfabCellularIoT::sendATCommOnce(const char *comm)
{
  BG96_AT.print(comm);
  BG96_AT.print("\r");
  //DEBUG.println(comm);
}

// function for sending at command to BG96_AT.
void SixfabCellularIoT::sendATComm(const char *command, const char *desired_reponse)
{
  uint32_t timer;
  char response[AT_RESPONSE_LEN]; // module response for AT commands. 

  memset(response, 0 , AT_RESPONSE_LEN);
  BG96_AT.flush();

  sendATCommOnce(command);
 
  timer = millis();
   while(true){
    if(millis()-timer > timeout){
      sendATCommOnce(command);
      timer = millis();
    }
    char c;
    int i = 0; 

    while(BG96_AT.available()){
      c = BG96_AT.read();
      DEBUG.write(c);
      response[i++]=c;
      delay(2);
      }
      if(strstr(response, desired_reponse)){
        memset(response, 0 , strlen(response));
        break;
      }    
  }
}

// function for reset BG96_AT module
void SixfabCellularIoT::resetModule()
{
}

// Function for save configurations that be done in current session. 
void SixfabCellularIoT::saveConfigurations()
{
  sendATComm("AT&W","OK\r\n");
}

// function for getting ip_address
const char* SixfabCellularIoT::getIPAddress()
{
  return ip_address;
}


// Function for getting IMEI number
char* SixfabCellularIoT::getIMEI()
{

}

// Function for getting firmware info
char* SixfabCellularIoT::getFirmwareInfo()
{

}
//Function for getting hardware info
char* SixfabCellularIoT::getHardwareInfo()
{

}

//Function for setting running mode.
void SixfabCellularIoT::setMode(uint8_t)
{

}

// function for setting ip_address
void SixfabCellularIoT::setIPAddress(char *ip)
{
  strcpy(ip_address, ip);
}

// function for getting domain_name
const char* SixfabCellularIoT::getDomainName()
{
  return domain_name;
}

// function for setting domain name
void SixfabCellularIoT::setDomainName(char *domain)
{
	strcpy(domain_name, domain);
}

// function for getting domain_name
const char* SixfabCellularIoT::getPort()
{
  return port_number;
}

// function for setting port
void SixfabCellularIoT::setPort(char *port)
{
	strcpy(port_number, port);
}

// get timout in ms
uint16_t SixfabCellularIoT::getTimeout()
{
  return timeout;
}

// set timeout in ms    
void SixfabCellularIoT::setTimeout(uint16_t new_timeout)
{
  timeout = new_timeout; 
}


/******************************************************************************************
 *** Network Service Functions ************************************************************
 ******************************************************************************************/ 

//
char* SixfabCellularIoT::getSignalQuality()
{
  
}

//
char* SixfabCellularIoT::getQueryNetworkInfo()
{

}

// connect to base station of operator
void SixfabCellularIoT::connectToOperator()
{
  DEBUG.println("Trying to connect base station of operator...");
  sendATComm("AT+CGATT?","+CGATT:1\r\n");
  sendATComm("AT+CSQ","OK\r\n"); 
}

/******************************************************************************************
 *** Call Service Functions ***************************************************************
 ******************************************************************************************/



/******************************************************************************************
 *** Short Message Service Functions ******************************************************
 ******************************************************************************************/



/******************************************************************************************
 *** Packet Domain Service Functions ******************************************************
 ******************************************************************************************/



/******************************************************************************************
 *** GNSS Functions ***********************************************************************
 ******************************************************************************************/



/******************************************************************************************
 *** TCP & UDP Protocols Functions ********************************************************
 ******************************************************************************************/

// fuction for sending data via udp.
void SixfabCellularIoT::sendDataUDP(const char *data)
{
  char compose[DATA_COMPOSE_LEN]="";
  char data_hex[DATA_COMPOSE_LEN];
  char data_len[DATA_LEN_LEN];
  
  for(int i= 0; i<strlen(data); i++){
    sprintf(data_hex+i*2, "%02X", data[i]);
    }  

  sprintf(data_len, "%d", strlen(data));
  strcat(compose, "AT+NSOST=0,");
  strcat(compose, ip_address);
  strcat(compose, ",");
  strcat(compose, port_number);
  strcat(compose, ",");
  strcat(compose, data_len);
  strcat(compose, ",");
  strcat(compose, data_hex);
  DEBUG.print("\n"); 
  DEBUG.println(compose);
  
  sendATComm(compose,"OK\r\n");
}

/******************************************************************************************
 *** MQTT Protocol Functions **************************************************************
 ******************************************************************************************/  



/******************************************************************************************
 *** Peripheral Devices' Functions : Read sensors - Set Relay and LEDs ********************
 ******************************************************************************************/  



/******************************************************************************************
 *** Peripheral Devices' Functions : Read sensors - Set Relay and LEDs ********************
 ******************************************************************************************/  

// 
void SixfabCellularIoT::readAccel(double* ax, double* ay, double* az)
{
  accel.read();
  *ax = accel.cx;
  *ay = accel.cy;
  *az = accel.cz;
}

//
double SixfabCellularIoT::readTemp()
{
  return hdc1080.readTemperature();
}

// 
double SixfabCellularIoT::readHum()
{
  return hdc1080.readHumidity();
}

//
double SixfabCellularIoT::readLux()
{
  return analogRead(ALS_PT19_PIN);
}

//
void SixfabCellularIoT::turnOnRelay()
{
  digitalWrite(RELAY, HIGH);
}

//
void SixfabCellularIoT::turnOffRelay()
{ 
  digitalWrite(RELAY, LOW);
}

//
uint8_t SixfabCellularIoT::readUserButton()
{
  digitalRead(USER_BUTTON);
}

//
void SixfabCellularIoT::turnOnUserLED()
{
  digitalWrite(USER_LED, HIGH);
}

//
void SixfabCellularIoT::turnOffUserLED()
{
  digitalWrite(USER_LED, LOW);
}


/******************************************************************************************
 *** Private Functions that be used in public methods, in order to ease the operations ****
 ******************************************************************************************/
