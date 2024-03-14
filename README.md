# DAI_Firebase_Proj
A simple firebase ESP32-C3 project to record the amount of calcium carbonate formed on a cathode over time

## Background
I am currently working with a company that aims to build electric reefs in Singapore to grow corals and hit Singapore's target of 100,000 corals. The electric reef pillars make use of electrolysis to deposit calcium carbonate on the cathode which then facilitates coral growth.

<p align="center">
  <img width="460" height="300" src="https://github.com/skywillow/DAI_Firebase_Proj/assets/154501032/a4655e67-031e-40d1-a96b-125b39dac5b6">
</p>

There is currently much testing being conducted with different set up variations. The material used for the anode and cathode, distance between them as well as shape of the cathode are being tested to find the most effective way for calcium carbonate to form. I aim to reduce the manual labour involved in this process by automating the data collection and tracking process. I will use the ESP32-C3 Dev Kit to create a simple project to track the rough amount of calcium carbonate being formed in order to notify the user when a significant amount of calcium carbonate has been formed. There will be a tag "Little Formation", "Some Formation" and "Much Formation" according to the recorded LDR readings to let the user know roughly how much calcium carbonate has been formed.

The data we will be collating includes the LDR analog readings, a corresponding tag as well as a time stamp. This is so that the time lapsed since testing started can also be calculated. With these data, users can then estimate the speed taken for the calcium carbonate to form as well as monitor the formation and can head down to check on it when necessary.

<p align="center">
  <img width="460" height="400" src="https://github.com/skywillow/DAI_Firebase_Proj/assets/154501032/6b92c319-f57d-4a4e-9d4e-3a973d931690">
</p>

<p align="center">
  <img width="460" height="400" src="https://github.com/skywillow/DAI_Firebase_Proj/assets/154501032/f7ea1225-e608-412b-935d-ae4a605ee929">
</p>

For demonstration purposes, I have made some assumptions but these can easily be adjusted and calibrated for use in actual experimentation.

1. Usually readings are taken over 5 days, however for this demonstration we will assume readings are taken over 15 seconds intervals. This time interval can easily be changed to the required interval.
2. The LDR values that corresponds to each given tag is an estimate, accurate calibration of the sensors is still required to reflect the amount of calcium carbonate that is being formed accurately
3. We assume the circuit is being placed outside the experiment tank and thus does not require waterproofing. Slight modification of the circuitry would be needed for actual application as it would have to be submerged underwater.

## Things you need for the Project
| Hardware Components  | Software |
| ------------- | ------------- |
| ESP32-C3 Dev Kit  | Arduino IDE  |
| LED Diode x 1  |  Google Firebase |
| Light Dependent Resistor (LDR) x 1  |   |
| Resistor 220 Ohm x 2  |   |
| Jumper Cables  |   |
| Breadboard  |   |

## Schematic Diagram

![image](https://github.com/skywillow/DAI_Firebase_Proj/assets/154501032/963d7efc-c55e-4c75-8883-26a818d5aeb8)


## Code

```
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
```

## Physical Set Up

## Application
