#ifndef EEPROM_Utils_h
#define EEPROM_Utils_h
#include "BluetoothSerial.h"
#include <WiFi.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#include "EEPROM.h"



const int BASE_ADDR = 0x0A;
const int IP_ADDR =BASE_ADDR+ 0;           // IP address on EEPROM  
const int SSID_LEN_ADDR = IP_ADDR + 4;     // SSID length address on EEPROM
const int PWD_LEN_ADDR = SSID_LEN_ADDR +1; // Password length address on EEPROM
const int SSID_ADDR = PWD_LEN_ADDR + 1;    // SSID address on EEPROM
const int PWD_ADDR = SSID_ADDR + 64;       // Password address on EEPROM
const int PORT_ADDR = PWD_ADDR + 64;       // Port address on EEPROM

struct udp_data{
  IPAddress server;
  char * ssid;
  char * password;
  int ssidL;
  int pwdL;
  int localPort;
};


void get_udp_data(udp_data* data);

void print_udp_data(udp_data data);

void setSSID (String SSID);

void setIP (IPAddress IP);

void setPORT (byte p1, byte p2);

void parseMy(String msg,BluetoothSerial *SBT);

void  getDatafromSerial(BluetoothSerial *SBT);

void setupWiFi(udp_data ROS_data);

#endif