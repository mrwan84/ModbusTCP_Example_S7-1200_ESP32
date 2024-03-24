/*

*/
#include <WiFi.h>
#include <ModbusIP_ESP8266.h>
//-------------------------------------------------------
const char* ssid = ""; // Your SSID
const char* password = ""; //Your Password
void setup_wifi() 
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to wifi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//--------------------------
const int REG = 0;               // Modbus Hreg Offset
IPAddress remote(192, 168, 200, 11);  // Address of Modbus Slave device /PLC
ModbusIP mb;  //ModbusIP object

//===============================L298N Setting================================================
// Motor 
int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin = 14; 
int potPin = 34;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 0;


void setup() {
  Serial.begin(115200);
   setup_wifi();
  mb.client();
  //==============================================================
   // sets the pins as outputs for L298N:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  
  // configure PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH); 
}

uint16_t res = 0;

void loop() {
  if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
    mb.readHreg(remote, REG, &res);  // Initiate Read Coil from Modbus Slave
  } else {
    mb.connect(remote);           // Try to connect if no connection
  }
  mb.task();                      // Common local Modbus task
  delay(100);                     // Pulling interval
  
    Serial.print("PLC Modbus Value: ");
    Serial.println(res); // Display Slave register value 
  
    dutyCycle = map(res, 0, 100, 100, 255);
    ledcWrite(pwmChannel, dutyCycle);   
    Serial.print("duty cycle: ");
    Serial.println(dutyCycle);
  
}
