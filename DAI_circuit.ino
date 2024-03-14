/*
  Rui Santos
  Complete project details at our blog.
    - ESP32: https://RandomNerdTutorials.com/esp32-firebase-realtime-database/
    - ESP8266: https://RandomNerdTutorials.com/esp8266-nodemcu-firebase-realtime-database/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Based in the RTDB Basic Example by Firebase-ESP-Client library by mobizt
  https://github.com/mobizt/Firebase-ESP-Client/blob/main/examples/RTDB/Basic/Basic.ino
*/
// above is included citation for the reference for the esp32 firebase realtime database code

/*
Before continuing you have to install 
1) the Firebase Arduino Client Library for ESP8266 and ESP32 into your Arduino IDE by searching under "Tools" and then "Manage Library"
2) The ESP32Time library by searching under "Tools" and then "Manage Library" 
*/

#include <WiFi.h> //Include the Wifi library
#include <Firebase_ESP_Client.h> //Include the 
#include "addons/TokenHelper.h" //Provide the token generation process info.
#include "addons/RTDBHelper.h" //Provide the RTDB payload printing info and other helper functions.
#include <ESP32Time.h> //Include the time library

// Insert your network credentials (ie wifi network name and password)
#define WIFI_SSID "phine"
#define WIFI_PASSWORD "haniisbae"

// Insert Firebase project API Key (This can be obtained by heading to your Firebase's project settings)
#define API_KEY "AIzaSyBfxlqpUi4NzVRKS4A66XJG2K932IgiYVw"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://dai-esp-default-rtdb.asia-southeast1.firebasedatabase.app/" 

// The PIN number on the ESP that is connected to the circuit
#define LED_PIN 2
#define LDR_PIN 3

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;

FirebaseConfig config;

unsigned long sendDataPrevMillis = 0; //read and write to firebase database at specified interval
int ldrData = 0; //ldrData variable
bool signupOK = false; //successful signup
int voltage = 0.0; //voltage variable
const char* ntpServer = "sg.pool.ntp.org"; //for ntpserver to retrieve time
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 28800; //GMT difference

//ESP32Time rtc;
ESP32Time rtc(3600);  // offset in seconds GMT+1

void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300); //print dot every 300s
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){ //anonymous user sign up
    Serial.println("Signup ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  /*---------set LED pin output---------------*/
  pinMode (LED_PIN, OUTPUT);

  /*---------set time with NTP---------------*/
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)){
    rtc.setTimeStruct(timeinfo); 
  
  }
}

void loop(){
  digitalWrite (LED_PIN, HIGH);

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // assign analog data to variables
    ldrData = analogRead(LDR_PIN);
    voltage = analogReadMilliVolts(LDR_PIN);

    if(Firebase.RTDB.setInt(&fbdo,"Sensor/ldr_data",ldrData)){ //store data to Firebase
      Serial.println(); Serial.print(ldrData);
      Serial.print(" - sucessfully saved to: " + fbdo.dataPath());
      Serial.println(" ("+ fbdo.dataType() + ")");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }
    
    if(Firebase.RTDB.setInt(&fbdo,"Sensor/voltage",voltage)){ //store data to Firebase
      Serial.println(); Serial.print(voltage);
      Serial.print(" - sucessfully saved to: " + fbdo.dataPath());
      Serial.println(" ("+ fbdo.dataType() + ")");
    }else{
      Serial.println("FAILED: " + fbdo.errorReason());
    }

    Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));   // (String) returns time with specified format 
    String s = rtc.getTime("%A, %B %d %Y %H:%M:%S"); // store the "String" type data as variable s
    Firebase.RTDB.setString(&fbdo,"Sensor/time",s); // store to firebase

// To print and record a tag for the corresponding LDR data recorded 
    if (ldrData > 250) {
      String result = "Little Formation" ;
      Serial.print(result);
      Firebase.RTDB.setString(&fbdo,"Sensor/Amount Formed",result); 
    }

    if (ldrData > 200 && ldrData < 250) {
      String result = "Some Formation" ;
      Serial.print(result);
      Firebase.RTDB.setString(&fbdo,"Sensor/Amount Formed",result); 
    }

    if (ldrData < 200) {
      String result = "Much Formation" ;
      Serial.print(result);
      Firebase.RTDB.setString(&fbdo,"Sensor/Amount Formed",result); 
    }
    delay(15000);
  }
}