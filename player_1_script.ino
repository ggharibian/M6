/*

  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.

  This sketch will create a new access point (with no password).

  It will then launch a new server and print out the IP address

  to the Serial monitor. From there, you can open that address in a web browser

  to turn on and off the LED on pin 13.

  If the IP address of your board is yourAddress:

    http://yourAddress/H turns the LED on

    http://yourAddress/L turns it off

  created 25 Nov 2012
-
  by Tom Igoe

  adapted to WiFi AP by Adafruit

 */

#include <Arduino_LSM6DS3.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;

uint sampling_rate = 1000;

unsigned long start_time;

WiFiServer server(80);

void setup() {

  //Initialize serial and wait for port to open:

  Serial.begin(9600);

  Serial.println("IMU Sensor Init");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  // setAccelODR(5);
  // XL_HM_MODE;

  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:

  if (WiFi.status() == WL_NO_MODULE) {

    Serial.println("Communication with WiFi module failed!");

    // don't continue

    while (true);

  }

  String fv = WiFi.firmwareVersion();

  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {

    Serial.println("Please upgrade the firmware");

  }

  // by default the local IP address of will be 192.168.4.1

  // you can override it with the following:

  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);

  Serial.print("Creating access point named: ");

  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:

  status = WiFi.beginAP(ssid, pass);

  if (status != WL_AP_LISTENING) {

    Serial.println("Creating access point failed");

    // don't continue

    while (true);

  }

  // wait 10 seconds for connection:

  // start the web server on port 80

  server.begin();

  Serial.println("SERVER CREATED");

  // you're connected now, so print out the status

  start_time = millis();
}

void loop() {

  float ax, ay, az, avg_ax, avg_ay, avg_az;
  float gx, gy, gz, avg_gx, avg_gy, avg_gz;
  uint count = 0;

  if (status != WiFi.status()) {
    status = WiFi.status();
  }

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.

          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)

            // and a content-type so the client knows what's coming, then a blank line:

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Access-Control-Allow-Origin: *");
            client.println();

            // the content of the HTTP response follows the header:
            if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
              IMU.readAcceleration(ax, ay, az);
              IMU.readGyroscope(gx, gy, gz);
              client.print("{\"ax\":");
              client.print(ax);
              client.print(",\"ay\":");
              client.print(ay);
              client.print(",\"az\":");
              client.print(az);
              client.print(",\"gx\":");
              client.print(gx);
              client.print(",\"gy\":");
              client.print(gy);
              client.print(",\"gz\":");
              client.print(gz);
              client.println("}");
            }
            // client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
            // client.print("Click <a href=\"/L\">here</a> turn the LED off<br>");

            // The HTTP response ends with another blank line:

            // break out of the while loop:

            break;

          }

          else {      // if you got a newline, then clear currentLine:

            currentLine = "";

          }

        }

        else if (c != '\r') {    // if you got anything else but a carriage return character,

          currentLine += c;      // add it to the end of the currentLine

        }

      }

    }

    // close the connection:

    client.stop();
    delay(1);
  }
}