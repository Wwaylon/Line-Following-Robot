#include <iostream>
#include <string>
#include <SPI.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include "linefollowing.h"
#include "rfid.h"
#include "buttons.h"
#include "globals.h"
#define led_pin LED_BUILTIN //ignore for debug purposes
static const BaseType_t pro_cpu = 0;
static const BaseType_t app_cpu = 1;

WebSocketsServer webSocket = WebSocketsServer(80);

const char* ssid = "";
const char* password = "";

int command = 0;
bool sync;
int target_table =-1;
int tables = 0 ;
bool deliver_flag = false;
bool search_start = false;
bool wait = false;

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  // Figure out the type of WebSocket event
  static char str[10];
  static bool firstcomm = true;
  int c_payload;
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      firstcomm = true;      
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
      }
      break;

    // Echo text message back to client
    case WStype_TEXT:
    //if payload is numeric
      if (payload[0] >= '0' && payload[0] <= '9') {
        c_payload = atoi((char*)payload);
        if(firstcomm)
        {
          firstcomm =false;
          tables = c_payload;
          Serial.print("Payload = ");
          Serial.println(tables);
          webSocket.sendTXT(num, "Received data");
        }
        else {
          //check if payload is a valid table number
          if (c_payload <= tables) {
            //send the table number to the robot
            target_table = c_payload;
            deliver_flag= true;
            webSocket.sendTXT(num, "delivering to table");
          }
          else {
            //send error message to the client
            webSocket.sendTXT(num, "error");
          }
        }
      }
      else if (strcmp((char*)payload, "sync") == 0) {
        sync = true;
        //send tables to the client
        String str = String(tables);
        webSocket.sendTXT(num, str);
      }
      else if (strcmp((char*)payload, "reset") == 0) {
        //sync = true;
        //send tables to the client
        tables = 0;
        webSocket.sendTXT(num, "Reset Table complete");
      }
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}


void main_task(void *parameters)
{
  static char str[10];
  static bool broadcast=false;  
  while(1) //wait state
    {
      webSocket.loop();
      //wait = true;
      if(broadcast)
      {
        broadcast = false;
        webSocket.broadcastTXT("done");        
      }      
      
      if(deliver_flag)
      {
        wait = false;
        deliver_flag= false;
        command =1;
        while(1) //deliver state
        {
          if (resume_flag && command !=-1) //Resume movement
          {
            command =1;
            resume_flag =false; 
            search_start = true;    
          }
          if (command == -1)
          {
            search_start = false;
            deliver_flag=false;
            broadcast = true;
            break;            
          }

          webSocket.loop();
          rfid_read();
          line_following(command);
        }
      }
      if (setup_flag) //enter setup mode
      {
        wait = false;
        Serial.println("Setup mode");
        command = 0;
        line_following(command);
        setup_flag= false;
        while(1) //setup state
        {   
          webSocket.loop();   
          rfid_write();
          //Serial.println("setup");
          if(setup_flag==true)
          {
            setup_flag=false;
            resume_flag=false;         
            break;            
          }         
        }          
      }
      //line_following(command); //execute command
      //Serial.println(digitalRead(buttonPin));
      //rfid_read();
    }
}
void task2(void *parameters)
{
  while(1)
  {
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}



void setup()
{
    Serial.begin(115200);
    SPI.begin();
    // Wait a moment to start (so we don't miss Serial output)
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("Starting RTOS");
    motors_init();//init motors
    pinMode(led_pin, OUTPUT); //for debug purposes
    buttons_init();
    rfid_init();    
    
    Serial.println("Connecting");
    WiFi.begin(ssid, password);
    while ( WiFi.status() != WL_CONNECTED ) {
      delay(500);
      Serial.print(".");
    }

    // Print our IP address
    Serial.println("Connected!");
    Serial.print("My IP address: ");
    Serial.println(WiFi.localIP());

    // Start WebSocket server and assign callback
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);

    rotateMotor(0,0); 

    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input

    xTaskCreatePinnedToCore(main_task, "line_following", 10000, NULL, 1, NULL, app_cpu);
    xTaskCreatePinnedToCore(task2, "rfid_task", 10000, NULL, 1, NULL, pro_cpu);


}

void loop()
{

}