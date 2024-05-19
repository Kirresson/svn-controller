/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

// MAC adress of the sender
uint8_t broadcastAddress[] = {0x08, 0xF9, 0xE0, 0x67, 0xC8, 0xC3};
int accessGranted;
bool dataReceived;
bool dataSent;

// Define variables to store incoming readings
char senderMAC[32];
char memberToken[7];

// Variable to store if sending data was successful
String success;

//Structure example to receive data
//Must match the sender structure
typedef struct struct_message_receive {
    char senderMAC[35];
    char memberToken[7];
} struct_message_receive;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message_send {
    char controllerMAC[35];
    int accessGranted;
} struct_message_send;

// Create a struct_message called controllerResponse to hold sensor readings
struct_message_send controllerResponse;

// Create a struct_message to hold incoming sensor readings
struct_message_receive scannedCard;

esp_now_peer_info_t peerInfo;


void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  Serial.println(WiFi.macAddress());
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
     Serial.println("Failed to add peer");
     return;
  }
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
 
  if(dataReceived){
    replySender();
    dataReceived = false;
  }

  if(dataSent){
    dataSent = false;
  }
}


// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
  dataSent = true;
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&scannedCard, incomingData, sizeof(scannedCard));
  //Serial.print("Bytes received: ");
  //Serial.println(len);
  //senderMAC = scannedCard.senderMAC;
  //memberToken = scannedCard.memberToken;
  printReceived();
  dataReceived = true;
}
 
void replySender(){
// Set values to send
  char testToken[7];
  strcpy(controllerResponse.controllerMAC, "0x08, 0x3A, 0xF2, 0x31, 0x9E, 0x68");
  strcpy(testToken, "9F4791");
  if(strcmp(scannedCard.memberToken, testToken)== 0){
    controllerResponse.accessGranted = 1;
  }
  else{
    controllerResponse.accessGranted = 0;
  }

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &controllerResponse, sizeof(controllerResponse));
  
  // Check if sent
  if (result == ESP_OK) {
    Serial.println("Sent with success");
    Serial.println("Sent: ");
    Serial.println(controllerResponse.controllerMAC);
    Serial.println(controllerResponse.accessGranted);
  }
  else {
    Serial.println("Error sending the data");
  }

}


void printReceived(){
  // Display Readings in Serial Monitor
  Serial.println("Incoming data from scanned card");
  Serial.print("Sender MAC: ");
  Serial.println(scannedCard.senderMAC);
  Serial.print("Membertoken: ");
  Serial.println(scannedCard.memberToken);
  Serial.println();
}