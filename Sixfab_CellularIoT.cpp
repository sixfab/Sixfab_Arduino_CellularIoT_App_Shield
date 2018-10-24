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
  pinMode(RELAY, OUTPUT);
  pinMode(USER_BUTTON, INPUT);
  pinMode(ALS_PT19_PIN, INPUT);
  
  enable();

  // setting serials
  BG96_AT.begin(9600);
  DEBUG.begin(115200);
  GNSS.begin(115200);

  while(getModemStatus()){
    DEBUG.println(getModemStatus());
    powerUp();  
    DEBUG.println(getModemStatus());
  }
  
  DEBUG.println("Module initializing");
  delay(500); // wait until module ready.

  // HDC1080 begin
  hdc1080.begin(0x40);
  // mma8452q init 
  accel.init();
  delay(3000);
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
  while(getModemStatus()){}
}

// power up or down BG96 module
uint8_t SixfabCellularIoT::getModemStatus()
{
  pinMode(STATUS,INPUT);
  delay(10);
  return digitalRead(STATUS);
}

// send at comamand to module
void SixfabCellularIoT::sendATCommOnce(const char *comm)
{
  BG96_AT.print(comm);
  BG96_AT.print("\r");
  //DEBUG.println(comm);
}

// function for sending at command to BG96_AT.
const char* SixfabCellularIoT::sendATComm(const char *command, const char *desired_reponse)
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
        return response;
        memset(response, 0 , strlen(response));
        break;
      }    
  }
}

// function for sending data to BG96_AT.
const char* SixfabCellularIoT::sendDataComm(const char *command, const char *desired_reponse)
{
  uint32_t timer;
  char response[AT_RESPONSE_LEN]; // module response for AT commands. 

  memset(response, 0 , AT_RESPONSE_LEN);
  BG96_AT.flush();

  BG96_AT.print(command);
 
  timer = millis();
   while(true){
    if(millis()-timer > timeout){
      BG96_AT.print(command);
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
        return response;
        memset(response, 0 , strlen(response));
        break;
      }    
  }
}

// function for reset BG96_AT module
void SixfabCellularIoT::resetModule()
{
  saveConfigurations();
  delay(200);

  digitalWrite(BG96_ENABLE,LOW);
  delay(200);
  digitalWrite(BG96_ENABLE,HIGH);
  delay(200);

  powerUp();
}

// Function for save configurations that be done in current session. 
void SixfabCellularIoT::saveConfigurations()
{
  sendATComm("AT&W","OK\r\n");
}

// Function for getting IMEI number
const char* SixfabCellularIoT::getIMEI()
{
  return sendATComm("AT+CGSN","OK\r\n");
}

// Function for getting firmware info
const char* SixfabCellularIoT::getFirmwareInfo()
{
  return sendATComm("AT+CGMR","OK\r\n");
}

//Function for getting hardware info
const char* SixfabCellularIoT::getHardwareInfo()
{
  return sendATComm("AT+CGMM","OK\r\n");
}


// Function for setting GSM Band
void SixfabCellularIoT::setGSMBand(const char *gsm_band)
{
  strcpy(compose, "AT+QCFG=\"band\",");
  strcat(compose, gsm_band);
  strcat(compose, ",");
  strcat(compose, LTE_NO_CHANGE);
  strcat(compose, ",");
  strcat(compose, LTE_NO_CHANGE);

  sendATComm(compose,"OK\r\n");
  clear_compose();
}

// Function for setting Cat.M1 Band
void SixfabCellularIoT::setCATM1Band(const char *catm1_band)
{
  strcpy(compose, "AT+QCFG=\"band\",");
  strcat(compose, GSM_NO_CHANGE);
  strcat(compose, ",");
  strcat(compose, catm1_band);
  strcat(compose, ",");
  strcat(compose, LTE_NO_CHANGE);

  sendATComm(compose, "OK\r\n");
  clear_compose();
}

// Function for setting NB-IoT Band
void SixfabCellularIoT::setNBIoTBand(const char *nbiot_band)
{
  strcpy(compose, "AT+QCFG=\"band\",");
  strcat(compose, GSM_NO_CHANGE);
  strcat(compose, ",");
  strcat(compose, LTE_NO_CHANGE);
  strcat(compose, ",");
  strcat(compose, nbiot_band);

  sendATComm(compose,"OK\r\n");
  clear_compose();
}

// Function for getting current band settings
const char* SixfabCellularIoT::getBandConfiguration()
{
  return sendATComm("AT+QCFG=\"band\"","OK\r\n");
}


// Function for setting scramble feature configuration 
void SixfabCellularIoT::setScrambleConf(const char *scramble)
{
  strcpy(compose, "AT+QCFG=\"nbsibscramble\",");
  strcat(compose, scramble);
  sendATComm(compose,"OK\r\n");
  clear_compose();
}

//Function for setting running mode.
void SixfabCellularIoT::setMode(uint8_t mode)
{
  if(mode == AUTO_MODE){
    sendATComm("AT+QCFG=\"nwscanseq\",00,1","OK\r\n");
    sendATComm("AT+QCFG=\"nwscanmode\",0,1","OK\r\n");
    sendATComm("AT+QCFG=\"iotopmode\",2,1","OK\r\n");
    DEBUG.println("Modem configuration : AUTO_MODE");
    DEBUG.println("*Priority Table (Cat.M1 -> Cat.NB1 -> GSM)");
  }else if(mode == GSM_MODE){
    sendATComm("AT+QCFG=\"nwscanseq\",01,1","OK\r\n");
    sendATComm("AT+QCFG=\"nwscanmode\",1,1","OK\r\n");
    sendATComm("AT+QCFG=\"iotopmode\",2,1","OK\r\n");
    DEBUG.println("Modem configuration : GSM_MODE");
  }else if(mode == CATM1_MODE){
    sendATComm("AT+QCFG=\"nwscanseq\",02,1","OK\r\n");
    sendATComm("AT+QCFG=\"nwscanmode\",3,1","OK\r\n");
    sendATComm("AT+QCFG=\"iotopmode\",0,1","OK\r\n");
    DEBUG.println("Modem configuration : CATM1_MODE");
  }else if(mode == CATNB1_MODE){
    sendATComm("AT+QCFG=\"nwscanseq\",03,1","OK\r\n");
    sendATComm("AT+QCFG=\"nwscanmode\",3,1","OK\r\n");
    sendATComm("AT+QCFG=\"iotopmode\",1,1","OK\r\n");
    DEBUG.println("Modem configuration : CATNB1_MODE ( NB-IoT )");
  }
}

// function for getting ip_address
const char* SixfabCellularIoT::getIPAddress()
{
  return ip_address;
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
const char* SixfabCellularIoT::getSignalQuality()
{
  return sendATComm("AT+CSQ","OK\r\n");
}

//
const char* SixfabCellularIoT::getQueryNetworkInfo()
{
  return sendATComm("AT+QNWINFO","OK\r\n");
}

// connect to base station of operator
void SixfabCellularIoT::connectToOperator()
{
  DEBUG.println("Trying to connect base station of operator...");
  sendATComm("AT+CGATT?","+CGATT: 1\r\n");
  
  getSignalQuality(); 
}

/******************************************************************************************
 *** GNSS Functions ***********************************************************************
 ******************************************************************************************/

// Function for turning on GNSS
void SixfabCellularIoT::turnOnGNSS()
{
  sendATComm("AT+QGPS=1","OK\r\n");
}


// Function for turning of GNSS
void SixfabCellularIoT::turnOffGNSS()
{
  sendATComm("AT+QGPSEND","OK\r\n");
}


// Function for getting fixed location 
const char* SixfabCellularIoT::getFixedLocation()
{
  return sendATComm("AT+QGPSLOC?","+QGPSLOC:");
} 

/******************************************************************************************
 *** TCP & UDP Protocols Functions ********************************************************
 ******************************************************************************************/

// function for configurating and activating TCP context 
void SixfabCellularIoT::activateContext()
{
  sendATComm("AT+QICSGP=1","OK\r\n"); 
  delay(1000);
  sendATComm("AT+QIACT=1","\r\n");
}

// function for deactivating TCP context 
void SixfabCellularIoT::deactivateContext()
{
  sendATComm("AT+QIDEACT=1","\r\n");
}

// function for connecting to server via TCP
// just buffer access mode is supported for now.
void SixfabCellularIoT::connectToServerTCP()
{
  strcpy(compose, "AT+QIOPEN=1,1");
  strcat(compose, ",\"TCP\",\"");
  strcat(compose, ip_address);
  strcat(compose, "\",");
  strcat(compose, port_number);
  strcat(compose, ",0,0");
  sendATComm(compose,"OK\r\n");

  sendATComm("AT+QISTATE=0,1","OK\r\n");
  clear_compose();
}

// fuction for sending data via tcp.
// just buffer access mode is supported for now.
void SixfabCellularIoT::sendDataTCP(const char *data)
{
  char data_len[4];
  sprintf(data_len, "%d", strlen(data));

  strcpy(compose, "AT+QISEND=1,");
  strcat(compose, data_len);

  sendATComm(compose,">");
  sendATComm(data,"SEND OK");
  clear_compose();
}

// function for sending data to BG96_AT.
void SixfabCellularIoT::sendDataSixfabConnect(const char *server_url, const char *api_key, const char *data )
{
  timeout = 20000;

  char len[4];
  
  clear_compose();
  
  strcpy(compose, "AT+QHTTPCFG=\"contextid\",1");
  sendATComm(compose,"OK\r\n");
  
  strcpy(compose, "AT+QHTTPCFG=\"requestheader\",1");
  sendATComm(compose,"OK\r\n");
  
  strcpy(compose, "AT+QHTTPCFG=\"responseheader\",1");
  sendATComm(compose,"OK\r\n");
  
  strcpy(url, "https://");
  strcat(url, server_url);
  strcat(url, "/sixfabStage/");
  
  
  sprintf(len, "%d", strlen(url));
  
  
  strcpy(compose, "AT+QHTTPURL=");
  strcat(compose, len);
  strcat(compose, ",80");
  sendATComm(compose,"CONNECT\r\n");
  sendDataComm(url, "OK\r\n");
  
  sprintf(len, "%d", strlen(data));
  
 
  int payload_len = 0;
  
  strcpy(compose,"POST /sixfabStage/ HTTP/1.1\r\nHost: ");
  payload_len += strlen(compose);
  
  payload_len += strlen(server_url);
  
  strcpy(compose, "\r\nx-api-key: ");
  payload_len += strlen(compose);
  
  payload_len += strlen(api_key);
  
  strcpy(compose, "\r\nContent-Type: application/json\r\nContent-Length: ");
  payload_len += strlen(compose);
  
  payload_len += strlen(len);
  
  strcpy(compose, "\r\n\r\n");
  payload_len += strlen(compose);
  
  payload_len += strlen(data);
  
  sprintf(len, "%d", payload_len);
  
  
  strcpy(compose, "AT+QHTTPPOST=");
  strcat(compose, len);
  strcat(compose, ",60,60");
  sendATComm(compose,"CONNECT\r\n");
  
  
  strcpy(compose,"POST /sixfabStage/ HTTP/1.1\r\nHost: ");
  BG96_AT.print(compose);
  
  strcpy(compose, server_url);
  BG96_AT.print(compose);
  
  strcpy(compose, "\r\nx-api-key: ");
  BG96_AT.print(compose);
  
  strcpy(compose, api_key);
  BG96_AT.print(compose);
  
  strcpy(compose, "\r\nContent-Type: application/json\r\nContent-Length: ");
  BG96_AT.print(compose);
  
  sprintf(len, "%d", strlen(data));
  strcpy(compose, len);
  BG96_AT.print(compose);
  
  strcpy(compose, "\r\n\r\n");
  BG96_AT.print(compose);
  
  
  strcpy(compose, data);
  sendDataComm(compose, "OK\r\n");
  

  clear_compose();
}

// function for sending data to BG96_AT.
void SixfabCellularIoT::sendDataIFTTT(const char *eventName, const char *api_key, const char *data )
{
  timeout = 20000;

  char len[4];
  
  clear_compose();
  
  strcpy(compose, "AT+QHTTPCFG=\"contextid\",1");
  sendATComm(compose,"OK\r\n");
  
  strcpy(compose, "AT+QHTTPCFG=\"requestheader\",1");
  sendATComm(compose,"OK\r\n");
  
  strcpy(url, "https://maker.ifttt.com/trigger/");
  strcat(url, eventName);
  strcat(url, "/with/key/");
  strcat(url, api_key);
  
  sprintf(len, "%d", strlen(url));
  
  strcpy(compose, "AT+QHTTPURL=");
  strcat(compose, len);
  strcat(compose, ",80");
  sendATComm(compose,"CONNECT\r\n");
  sendDataComm(url, "OK\r\n");
  
  
  sprintf(len, "%d", strlen(url));
  
  
  int payload_len = 0;
  
  strcpy(compose,"POST /trigger/");
  payload_len += strlen(compose);
  
  strcpy(compose, eventName);
  payload_len += strlen(compose);
  
  strcpy(compose, "/with/key/");
  payload_len += strlen(compose);
  
  strcpy(compose, api_key);
  payload_len += strlen(compose);
  
  strcpy(compose," HTTP/1.1\r\nHost: ");
  payload_len += strlen(compose);
  
  strcpy(compose,"maker.ifttt.com\r\n");
  payload_len += strlen(compose);
  
  strcpy(compose, "Content-Type: application/json\r\nContent-Length: ");
  payload_len += strlen(compose);
  
  payload_len += strlen(len);
  
  strcpy(compose, "\r\n\r\n");
  payload_len += strlen(compose);
  
  payload_len += strlen(data);
  
  sprintf(len, "%d", payload_len);
  
  
  strcpy(compose, "AT+QHTTPPOST=");
  strcat(compose, len);
  strcat(compose, ",60,60");
  sendATComm(compose,"CONNECT\r\n");
  
  
  strcpy(compose,"POST /trigger/");
  BG96_AT.print(compose);
  
  strcpy(compose, eventName);
  BG96_AT.print(compose);
  
  strcpy(compose, "/with/key/");
  BG96_AT.print(compose);
  
  strcpy(compose, api_key);
  BG96_AT.print(compose);
  
  strcpy(compose," HTTP/1.1\r\nHost: ");
  BG96_AT.print(compose);
  
  strcpy(compose,"maker.ifttt.com\r\n");
  BG96_AT.print(compose);
  
  strcpy(compose, "Content-Type: application/json\r\nContent-Length: ");
  BG96_AT.print(compose);
  
  sprintf(len, "%d", strlen(data));
  strcpy(compose, len);
  BG96_AT.print(compose);
  
  strcpy(compose, "\r\n\r\n");
  BG96_AT.print(compose);
  
  strcpy(compose, data);
  sendDataComm(compose, "OK\r\n");
  

  clear_compose();
}


// function for sending data to BG96_AT.
void SixfabCellularIoT::sendDataThingspeak(const char *api_key, const char *data )
{
  timeout = 20000;

  char len[4];
  
  clear_compose();
  
  strcpy(compose, "AT+QHTTPCFG=\"contextid\",1");
  sendATComm(compose,"OK\r\n");
  
  strcpy(compose, "AT+QHTTPCFG=\"requestheader\",0");
  sendATComm(compose,"OK\r\n");
  
  strcpy(url, "https://api.thingspeak.com/update?api_key=");
  strcat(url, api_key);
  strcat(url, "&");
  strcat(url, data);
  
  sprintf(len, "%d", strlen(url));
  
  strcpy(compose, "AT+QHTTPURL=");
  strcat(compose, len);
  strcat(compose, ",80");
  sendATComm(compose,"CONNECT\r\n");
  sendDataComm(url, "OK\r\n");
  
  sendATComm("AT+QHTTPGET=80","+QHTTPGET");

  clear_compose();
}

// function for connecting to server via UDP
void SixfabCellularIoT::startUDPService()
{
  char *port = "3005";

  strcpy(compose, "AT+QIOPEN=1,1,\"UDP SERVICE\",\"");
  strcat(compose, ip_address);
  strcat(compose, "\",0,");
  strcat(compose, port);
  strcat(compose, ",0");
  
  sendATComm(compose,"OK\r\n");
  clear_compose();

  sendATComm("AT+QISTATE=0,1","\r\n");
}

// fuction for sending data via udp.
void SixfabCellularIoT::sendDataUDP(const char *data)
{
  char data_len[5];
  sprintf(data_len, "%d", strlen(data));

  strcpy(compose, "AT+QISEND=1,");
  strcat(compose, data_len);
  strcat(compose, ",\"");
  strcat(compose, ip_address);
  strcat(compose, "\",");
  strcat(compose, port_number);

  sendATComm(compose,">");
  sendATComm(data,"SEND OK");
  clear_compose();
}

// function for closing server connection
void SixfabCellularIoT::closeConnection()
{
  sendATComm("AT+QICLOSE=1","\r\n");
}

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

