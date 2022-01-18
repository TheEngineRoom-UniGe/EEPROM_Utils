#include "EEPROM_Utils.h"


void get_udp_data(udp_data* data){
  // Reading IP address from EEPROM
  for (int i = 0 ;i<4; i ++){  
    data->server[i]=EEPROM.read(IP_ADDR+i);
  }  

  // Reading SSID from EEPROM
  data->ssidL = EEPROM.read(SSID_LEN_ADDR);
  data->ssid = new char[data->ssidL+1];
  for (int i = 0; i < data->ssidL; i++){ 
    data->ssid[i]=(char)EEPROM.read(SSID_ADDR+i);
  }
  data->ssid[data->ssidL] = 0;

  // Reading password from EEPROM
  data->pwdL = EEPROM.read(PWD_LEN_ADDR);
  data->password = new char[data->pwdL+1];
  for (int i = 0; i <data->pwdL; i++){
    data->password[i]=(char)EEPROM.read(PWD_ADDR+i);
  }
  data->password[data->pwdL] = 0;

  // Reading port from EEPROM
  data->localPort = EEPROM.read(PORT_ADDR)*100;
  data->localPort += EEPROM.read(PORT_ADDR+1);
}

void print_udp_data(udp_data data){
  // Print SSID
  Serial.print ("SSID: ");
  Serial.println(data.ssid);

  // Print password
  Serial.print("PWD: ");
  Serial.println (data.password); 

  // Print IP
  Serial.print("ROS IP: "); 
  for (int i = 0 ;i<3; i ++){  
    Serial.print (data.server[i]); 
    Serial.write (".");
  }
  Serial.println(data.server[3]); 

  // Print port
  Serial.print ("Local Port: ");  
  Serial.println (data.localPort);
}

void setPassword(String PWD){
  EEPROM.write(PWD_LEN_ADDR,(byte)PWD.length());
  for (int i = 0; i <PWD.length(); i++){ 
    EEPROM.write(PWD_ADDR+i,(byte)PWD[i]);
  }
  EEPROM.commit();
}

void setSSID (String SSID) { 
  EEPROM.write(SSID_LEN_ADDR,(byte)SSID.length());
  for (int i = 0; i <SSID.length(); i++){ 
    EEPROM.write(SSID_ADDR+i,(byte)SSID[i]);
  }
  EEPROM.commit();
  Serial.print ("saving SSID");
  Serial.print (SSID);
}

void setIP (IPAddress IP){
    for (int i =0 ; i <4 ; i ++){
      EEPROM.write(IP_ADDR+i,IP[i]); 
    }
    EEPROM.commit();
}

void setPORT (byte p1, byte p2){
    EEPROM.write(PORT_ADDR,p1); 
    EEPROM.write(PORT_ADDR+1,p2); 
    EEPROM.commit();
}


void parseMy(String msg, BluetoothSerial *SBT){
  int datalen; 
  String SSID;
  String PWD;

  Serial.print ("message length");
  Serial.print(msg.length());


  if(((msg.startsWith("IP")) && ((msg.endsWith("IP\n"))||(msg.endsWith("IP\r\n"))))){
    // Serial.println ("incorrect format, should be IP;000;000;000;000;IP , exiting ....");
    //return;
    //}
    
    if (msg.length() < 22){
      Serial.print("length is ");Serial.println (msg.length());
      Serial.println ("incorrect format,, exiting ....");
      return;
    }else{  // right format
      byte int1 = (byte)msg.substring(3,6).toInt(); 
      byte int2 = (byte)msg.substring(7,10).toInt(); 
      byte int3 = (byte)msg.substring(11,14).toInt(); 
      byte int4 = (byte)msg.substring(15,19).toInt(); 
      IPAddress ip2(int1,int2,int3,int4); 
  
      setIP(ip2);
      SBT->print("new IP") ; SBT->println (ip2);
    
    }
  }else if((msg.startsWith("SSID;")) && ((msg.endsWith(";SSID\n"))||(msg.endsWith(";SSID\r\n")))){
    if(msg.endsWith(";SSID\r\n")){
      datalen = msg.length()-7;
    }
    if (msg.endsWith(";SSID\n")){
      datalen = msg.length()-6;
    }
    SSID = msg.substring(5,datalen);
    setSSID ( SSID);
    SBT->print("SSID:");
    SBT->println (SSID);
  }else if((msg.startsWith("PWD;")) && ((msg.endsWith(";PWD\n"))||(msg.endsWith(";PWD\r\n")))){
    if(msg.endsWith(";PWD\r\n")){
      datalen = msg.length()-6;
    }
    if(msg.endsWith(";PWD\n")){
      datalen = msg.length() - 5 ;
    }
    PWD = msg.substring(4,datalen);
    setPassword(PWD);
    SBT->print("PWD:");
    SBT->println (PWD);
  }else if((msg.startsWith("PORT;")) && ((msg.endsWith(";PORT\n"))||(msg.endsWith(";PORT\r\n")))){
    if(msg.length() != 15){
      Serial.print("length is ");Serial.println (msg.length());
      Serial.println ("incorrect format,, exiting ....");
      return;
    }else{
      byte int1 = (byte)msg.substring(5,7).toInt();
      byte int2 = (byte)msg.substring(7,9).toInt();
      setPORT(int1, int2);
      SBT->print("localPort: ");
      SBT->println(msg.substring(5,9).toInt());

    }
  }
}

void  getDatafromSerial(BluetoothSerial *SBT){
  Serial.println("If you want to update the stored info:"); 
  Serial.println("1. Enter the new server address if you want to change it, format IP;000;000;000;000;IP"); 
  Serial.println("2. Enter the new network ssid if you want to change it, format SSID;networkssid;SSID"); 
  Serial.println("3. Enter the new network passdword if you want to change it, format PWD;networkssid;PWD");
  Serial.println("4. Enter the new port if you want to change it, format PORT;localPort;PORT");
  Serial.println("************************************************************************************"); 

  delay (4000);

  if (!SBT->available()){
    delay (1000);
  }
  while (SBT->available()) {
    String msg = SBT->readString();
    parseMy(msg,SBT);
  }

  while (Serial.available()) {
    String msg = Serial.readString();
    parseMy(msg,SBT);
  }
}

void setupWiFi(udp_data ROS_data){ 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ROS_data.ssid, ROS_data.password);
  //Print to serial to find out IP address and debugging
  Serial.print("\nConnecting to "); Serial.print(ROS_data.ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20){
    delay(500);
    Serial.print(" ... ");
  }
  delay (1500);
  if(i == 21){
    Serial.print("Could not connect to"); Serial.println(ROS_data.ssid);
    while(1) delay(500);
  }
  Serial.println("Ready!");
  Serial.print("Use ");
  IPAddress ip = WiFi.localIP();
  Serial.print(ip);
  Serial.println(" to access client");
}