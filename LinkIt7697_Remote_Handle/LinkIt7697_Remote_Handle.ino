#include <Wire.h> 
#include <LWiFi.h>
#include <WiFiUdp.h>
#include <FreeRTOS.h>
#include <task.h>
//wifi connection information
#define HostIP "Your.Server.IPAddress"
char ssid[] = "YourSSID"; //  your network SSID (name)
char pass[] = "YourPassword";    // your network password (use for WPA, or use as key for WEP)
String Name = "Student_1";
int HostPort =8765;

int status = WL_IDLE_STATUS;

unsigned int localPort = 2390;      // local port to listen on
char packetBuffer[255]; //buffer to hold incoming packet
char charBuf[50];
WiFiUDP Udp;
//btns , LED , score , LED state
int8_t Btns[]={17,16,15,14,13,12,11,10};// 
const int8_t BTN_SIZE = sizeof(Btns);
boolean state = false;
boolean buttonUp = true;
int8_t whichBtnIsClicked;
int packetSize=0;
int dataLen=0;

xTaskHandle xHandle; 

char *t[3];
int i=0;
char *test;


void setup() {
  
  whichBtnIsClicked=0;
  Serial.begin(115200);
  
  // put your setup code here, to run once:

  for(int8_t i=0;i<=7;i++){
    pinMode(Btns[i],INPUT);
//    digitalWrite(Btns[i],HIGH);
  }

  Serial.println(Name);
  wifiInit();

  xTaskCreate(
                        taskFunc,          /* Task function. */
                        "Listener",        /* String with name of task. */
                        10000,            /* Stack size in words. */
                        NULL,             /* Parameter passed as input of the task */
                        0,                /* Priority of the task. */
                        &xHandle);            /* Task handle. */

}
void taskFunc(void * parameter){
  
   while(1){
   whichBtnIsClicked = whichBtnsClicked();  //  看哪一個按鈕被按下
    
    if(whichBtnIsClicked>=0 && buttonUp==true) { // 其中一個按鈕被按下
    sendDataToServer(String(whichBtnIsClicked));  //送給
    buttonUp = false;
    }
    else if( buttonUp != true) {
      buttonUp = true;
    }
    delay(80);
    }
    vTaskDelete( NULL );
  
  }
void loop() {
  
  
  packetSize = Udp.parsePacket();
  if(packetSize > 0){
    dataLen = Udp.read(packetBuffer, 255);
    if (dataLen > 0) {
      packetBuffer[dataLen] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    
    test = strtok(packetBuffer, " ");
     
    while (test != NULL) {
       t[i]=test;
        test = strtok(NULL, " ");
        i++;
    }
    if(strcmp(t[0],"cmd")==0 && strcmp(t[1],"redirect")==0){
      HostPort = atoi(t[2]);
      Serial.println("Change Port!");
      }
    }
  delay(100); // Delay 時間可視情況調整   
 }
 void sendDataToServer(String data){
  data = Name+":"+data;
  Serial.println(data);
    data.toCharArray(charBuf, 50) ;
    Udp.beginPacket(HostIP, HostPort);
    Udp.write(charBuf);
    Udp.endPacket();
  }
  

void wifiInit(){
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);

  sendDataToServer("Hello");
  }

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

int whichBtnsClicked(){
  for(int i=0;i<BTN_SIZE;i++){
    if( digitalRead(Btns[i])!= HIGH){
      return i;
      }
    }
  return -1;//use flag to record which one is clicked
 }
  



