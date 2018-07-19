/*
  Sixfab_CellularIoT.h 
  -
  Library for Sixfab Arduino CellularIoT Shield and 
  Sixfab Arduino CellularIoT Application Shield.
  -
  Created by Yasin Kaya (selengalp), july 13, 2018.
*/

#ifndef _SIXFAB_CELLULAR_H
#define _SIXFAB_CELLULAR_H

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
    #include "pins.h"    
#endif

#include <stdio.h>
#include <string.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Sixfab_HDC1080.h>
#include <Sixfab_MMA8452Q.h>

// determine board type
// Arduino Geniuno / Uno or Mega
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #define GNSS Serial // 115200 baud rate
  #define DEBUG Serial // 115200 baud rate
// Tinylab, Arduino Leonardo or Micro  
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
  #define GNSS Serial1 // 115200 baud rate
  #define DEBUG Serial // 115200 baud rate
#endif

extern SoftwareSerial BG96_AT;

// enum for setMode function
enum {
    AUTO_MODE = 0,
    GSM_MODE,
    CATM1_MODE,
    CATNB1_MODE,
};

// Peripheral Pin Definations
#define USER_BUTTON 6
#define USER_LED 5
#define BG96_ENABLE 4
#define ALS_PT19_PIN A1
#define RELAY A0
#define BG96_POWERKEY A2 

// Constants  
#define TIMEOUT 1000
#define IP_ADDRESS_LEN 30
#define DOMAIN_NAME_LEN 50
#define PORT_NUMBER_LEN 8  
#define AT_COMM_LEN 200
#define AT_RESPONSE_LEN 400
#define DATA_COMPOSE_LEN 200
#define DATA_LEN_LEN 3  



class SixfabCellularIoT
{
  public:

    /*
    Default constructer with no parameter

    [no-return]
    ---
    [no-param]
    */
    SixfabCellularIoT();
    
/******************************************************************************************
 *** Base Functions : Set or Clear Hardwares - Status Controls - Helper AT Functions  *****
 ******************************************************************************************/
    
    /*
    Function for initializing module and peripherals
    This function do things below:
    * enables BG96 module
    * sets pin directions

    [no-return]
    ---
    [no-param]
    */
    void init(); // initialize

    /*
    Function for powering up BG96 module and all peripherals from voltage regulator 

    [no-return]
    ---
    [no-param]
    */
    void enable();
    
    /*
    Function for powering down BG96 module and all peripherals from voltage regulator 

    [no-return]
    ---
    [no-param]
    */
    void disable();

    /*
    Function for powering up or down BG96 module

    [no-return]
    ---
    [no-param]
    */
    void powerUp();

    /*
    Function for sending AT [param #1] command to BG96.
    
    [no-return]
    ---
    [param #1] : const char* AT command word
    */
    void sendATCommOnce(const char *);

    /*
    Function for sending AT [param #1] command to BG96. If the desired [param #2] 
    response isn't recevived, function resend the AT command wait a time as [timeout].
    
    [return] : const char* response of AT command that received from bg96 modem
    ---
    [param #1] : const char* AT command word
    [param #2] : const char* AT desired_response word
    [param #3] : const char* AT response word
    */
    const char* sendATComm(const char *, const char *); 

    /*
    Function for resetting bg96 module and all peripherals.

    [no-return]
    ---
    [no-param]
    */
    void resetModule();

   /*
    Function to save configurations that be done in current session.   
    
    [no-return]
    --
    [no-param]
    */
    void saveConfigurations();

    /*
    Function for getting IMEI number

    [return] : const char* IMEI number
    [no-param]
    */
    const char* getIMEI();

    /*
    Function for getting firmware info

    [return] : const char* firmware info
    ---
    [no-param]
    */
    const char* getFirmwareInfo();

    /*
    Function for getting hardware info

    [return] : const char* hardware info
    ---
    [no-param]
    */
    const char* getHardwareInfo();

    /*
    Function for setting running mode.
    
    [no-return]
    ---
    [param #1] : mode selector
        [MODE_AUTO] [0] --> Automatic *Priority Table (Cat.M1 -> Cat.NB1 -> GSM)
        [MODE_GSM] [1] --> GSM
        [MODE_CATM1] [2] --> Cat.M1
        [MODE_CATNB1] [3] --> Cat.NB1 (NB-IoT) 
    */
    void setMode(uint8_t);

    /*
    Function for getting described ip address

    [return] : const char* IP Address
    ---
    [no-param]
    */
    const char* getIPAddress();

    /*
    Function for setting desired ip address

    [no-return]
    ---
    [param #1] : char * desired ip address
    */
    void setIPAddress(char *);

    /*
    Function for getting described domain name

    [return] : const char* domain name
    ---
    [no-param]
    */
    const char* getDomainName();

    /*
    Function for setting desired domain name

    [no-return]
    ---
    [param #1] : char * desired domain name
    */
    void setDomainName(char *);

    /*
    Function for getting described port number

    [return] : const char* port number
    ---
    [no-param]
    */
    const char* getPort();

    /*
    Function for setting desired port number

    [no-return]
    ---
    [param #1] : char * desired port number
    */
    void setPort(char *);

    /*
    Function for getting described timeout

    [return] : uint16_t described timeout in ms
    ---
    [no-param]
    */
    uint16_t getTimeout();

    /*
    Function for setting desired timeot

    [no-return]
    ---
    [param #1] : uint16_t desired timeout in ms
    */
    void setTimeout(uint16_t);

/******************************************************************************************
 *** Network Service Functions ************************************************************
 ******************************************************************************************/ 

    /*
    Function for getting signal quality

    [return] : const char* signal quality
    ---
    [no-param]
    */
    const char* getSignalQuality();

    /*
    Function for getting query network info

    [return] : const char* query network info
    ---
    [no-param]
    */
    const char* getQueryNetworkInfo();

   /*
    Function for connecting to base station of operator

    [no-return]
    ---
    [no-param]
    */
    void connectToOperator();

/******************************************************************************************
 *** GNSS Functions ***********************************************************************
 ******************************************************************************************/

    /*
    Function for connecting to base station of operator

    [no-return]
    ---
    [no-param]
    */
    void turnOnGNSS();

    /*
    Function for connecting to base station of operator

    [no-return]
    ---
    [no-param]
    */
    void turnOffGNSS();

    /*
    Function for connecting to base station of operator

    [return] : Fixed location. Lattitude and Longitude
    ---
    [no-param]
    */
    const char* getFixedLocation();   
    

/******************************************************************************************
 *** TCP & UDP Protocols Functions ********************************************************
 ******************************************************************************************/
    /* 
    function for configurating and activating TCP context 

    [no-return]
    ---
    [no-param]
    */ 
    void activateContext();

    /* 
    function for deactivating TCP context

    [no-return]
    ---
    [no-param]
    */ 
    void deactivateContext();

    /* 
    function for connecting to server via TCP
    just buffer access mode is supported for now.

    [no-return]
    ---
    [no-param]
    */
    void connectToServerTCP();

    /* 
    // function for connecting to server via UDP

    [no-return]
    ---
    [no-param]
    */
    void startUDPService();;


    /* 
    Function for closing server connection
    
    [no-return]
    ---
    [no-param]
    */
    void closeConnection();

    /*
    Function for sending data via UDP protocol. 
    First use setIPAddress and setPort functions before 
    try to send data with this function.  

    [param #1] : const char* data word
    */
    void sendDataUDP(const char *);

    /*
    Function for sending data via TCP protocol. 
    First use setIPAddress and setPort functions before 
    try to send data with this function.  

    [param #1] : const char* data word
    */
    void sendDataTCP(const char *);

/******************************************************************************************
 *** Peripheral Devices' Functions : Read sensors - Set Relay and LEDs ********************
 ******************************************************************************************/    
    
    /* 
    Function for reading accelerometer values.
    
    [no-return]
    ---
    [param #1] : double* acceleration on x plane
    [param #2] : double* acceleration on y plane
    [param #y] : double* acceleration on z plane
    */
    void readAccel(double*, double*, double*);

    /* 
    Function for reading temperature.
    
    [return] : double temperature  (celcius)
    ---
    [no-param]
    */
    double readTemp();

    /* 
    Function for reading humidity.
    
    [return] : double humidity %x
    ---
    [no-param]
    */
    double readHum();

    /* 
    Function for reading raw adc data from light sensor.
    
    [return] : double light adc 0-1023
    ---
    [no-param]
    */
    double readLux();

    /* 
    Function for turning on relay.
    
    [no-return]
    ---
    [no-param]
    */
    void turnOnRelay();

    /* 
    Function for turning off relay.
    
    [no-return]
    ---
    [no-param]
    */
    void turnOffRelay();

    /* 
    Function for reading button position.
    
    [return] : uint8_t button position 0 or 1
    ---
    [no-param]
    */
    uint8_t readUserButton();

    /* 
    Function for turning on LED.
    
    [no-return]
    ---
    [no-param]
    */
    void turnOnUserLED();

    /* 
    Function for turning off LED.
    
    [no-return]
    ---
    [no-param]
    */
    void turnOffUserLED();

  private:
    char ip_address[IP_ADDRESS_LEN]; //ip address       
    char domain_name[DOMAIN_NAME_LEN]; // domain name   
    char port_number[PORT_NUMBER_LEN]; // port number 
    uint16_t timeout = TIMEOUT; // default timeout for function and methods on this library.

/******************************************************************************************
 *** Private Functions that be used in public methods, in order to ease the operations ****
 ******************************************************************************************/
 
};
#endif