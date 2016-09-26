//library DHT22
#include <DHT.h>
//library esp
#include <ESP8266WiFi.h>
//library bmp180
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

const char* ssid = "Anon";                                  //fill in your wifi name
const char* password = "89228922";                         //fill in your wifi password
const char* server = "128.199.210.91";
String SerialNumber = "ZgkL2LfL0Q";


#define DHTPIN 2 // what pin we’re connected to
WiFiClient client;
DHT dht(DHTPIN, DHT22,15);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

int sensorPin = A0;    // input for LDR and rain sensor
int enable1 = 15;      // enable reading LDR
int enable2 = 13;      // enable reading Rain sensor

int sensorValue1 = 0;  // variable to store the value coming from sensor LDR
int sensorValue2 = 0;  // variable to store the value coming from sensor Rain sensor

void connectWifi(){
  if(WiFi.status() != WL_CONNECTED){
  	WiFi.begin(ssid, password);
  
  	Serial.println();
  	Serial.println();
  	Serial.print("Connecting to ");
  	Serial.println(ssid);
  	Serial.println();
  
  	digitalWrite(LED_BUILTIN, HIGH); 
  		while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
  		  delay(500);
  		}
  	digitalWrite(LED_BUILTIN, LOW); 
    
  	Serial.println();
  	Serial.println("WiFi connected");
  	Serial.println();
  }
}

void setup() {
	// declare the enable and Pin as an OUTPUT:
	pinMode(enable1, OUTPUT);
	pinMode(enable2, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);

	Serial.begin(115200);
	delay(10);
	// test BMP085
	  if (!bmp.begin())
	  {
		Serial.print("problem detecting BMP");
		while (1);
	  }
	dht.begin(); 
	// connectWifi
	connectWifi();
}
void loop() {
  //----------------check wifi
  connectWifi();
	//--------------------------DHT22/DHT11-------------------------
	delay(500);
	float h = dht.readHumidity();
	float t = dht.readTemperature();

	if (isnan(h) || isnan(t)) {
		Serial.println("Failed to read from DHT sensor!");
		return;
	}

	Serial.print("Temperature:      ");
	Serial.print(t);
	Serial.print(" C");
	Serial.println();

	Serial.print("Humidity:         ");
	Serial.print(h);
	Serial.print(" %");
	Serial.println();

	//measure dew point with the temperature and the humidity

	double gamma = log(h/100) + ((17.625*t) / (243.04+t));
 	double dp = 243.04*gamma / (17.625-gamma);

	Serial.print("Dew point:        ");
	Serial.print(dp);
	Serial.print(" C");
	Serial.println();

	//--------------------------BMP180------------------------

	if(!bmp.begin()) {
		Serial.print("Failed to read from BMP sensor!!");
		while(1);
	}

	sensors_event_t event;
	bmp.getEvent(&event);

	Serial.print("Pressure:         ");
	Serial.print(event.pressure);
	Serial.println("hPa");
 
  //--------------------------LDR-------------------------

  digitalWrite(enable1, HIGH); 
  sensorValue1 = analogRead(sensorPin);
  sensorValue1 = constrain(sensorValue1, 300, 850); 
  sensorValue1 = map(sensorValue1, 300, 850, 0, 1023); 
  Serial.print("Light intensity:  ");
  Serial.println(sensorValue1);
  digitalWrite(enable1, LOW);
  delay(500);

  //--------------------------Rain Sensor-------------------------

  digitalWrite(enable2, HIGH); 
  delay(1000);
  sensorValue2 = analogRead(sensorPin);
  sensorValue2 = constrain(sensorValue2, 150, 440); 
  sensorValue2 = map(sensorValue2, 150, 440, 1023, 0); 
    if (sensorValue2 >300)
    sensorValue2  = 1; 
    else 
    sensorValue2 = 0;
  Serial.print("Rain value:       ");
  Serial.println(sensorValue2);
  delay(500);
  digitalWrite(enable2, LOW); 
  Serial.println("");
	//--------------------------ConnectSercver-------------------------

	if (client.connect(server,80)) { // "184.106.153.149" or api.thingspeak.com

		String  postStr = "&temp=";
		  postStr += String(t);
		  postStr +="&humidity=";
		  postStr += String(h);
		  postStr +="&dewpoint=";
		  postStr += String(dp);
		  postStr +="&pressure=";
		  postStr += String(event.pressure);
		  postStr +="&light=";
		  postStr += String(sensorValue1);
		  postStr +="&rain=";
		  postStr += String(sensorValue2);
		  postStr +="&SerialNumber=";
		  postStr += String(SerialNumber);
	  
		client.println("POST /weather HTTP/1.1");
		client.println("Host: 128.199.210.91");
		client.println("User-Agent: Arduino/1.0");
		client.println("Connection: close");
		client.println("Content-Type: application/x-www-form-urlencoded;");
		client.print("Content-Length: ");
		client.println(postStr.length());
		client.println();
		client.println(postStr);
	}
	client.stop();

	// 5 minute delay between updates
 
 Serial.println("   !!!!!Wait 5 minute");
 delay(60000);
 Serial.println("   !!!!Wait 4 minute");
 delay(60000);
 Serial.println("   !!!Wait 3 minute");
 delay(60000);
 Serial.println("   !!Wait 2 minute");
 delay(60000);
 Serial.println("   !Wait 1 minute");
 delay(60000);
 
 Serial.println("");
 Serial.println("");
}
