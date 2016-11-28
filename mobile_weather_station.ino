#include <Arduino.h>

#include <sim800.h>
#include <SoftwareSerial.h>

#include "arduino_sleep.h"
#include "report_server_connection.h"



// Data logging configuration.
#define LOGGING_FREQ_SECONDS   300       // Seconds to wait before a new sensor reading is logged.


char http_cmd[] = "GET exploreembedded.com/wiki/images/1/15/Hello.txt HTTP/1.0\r\n\r\n";
char buffer[512];

GPRS gprs;


void report_temperature()
{
  turn_on_sim800l();
  Serial.println("Turned on SIM800L");

  if (start_gprs_connection(gprs))
  {
    Serial.println("Connecting to mbed.org...");
  
    if (0 == gprs.connectTCP("exploreembedded.com", 80))
    {
      Serial.println("connect mbed.org success");
      Serial.println("waiting to fetch...");
      if(0 == gprs.sendTCPData(http_cmd))
      {
        gprs.serialDebug();
      }
      gprs.closeTCP();
      gprs.shutTCP();
    }
    else
    {
        Serial.println("connect error");
    }
  }
  else
  {
    Serial.println("Failed to connect to mobile network,");
  }

  Serial.println("Shutting down GPRS");
  shutdown_gprs(gprs);
}

void setup()
{
  Serial.begin(9600);
  while(!Serial);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Serial.println("Setup");

  report_temperature();

  configure_sleep(report_temperature, LOGGING_FREQ_SECONDS);
  
  Serial.println("Setup complete.");
}

void loop()
{
  manage_sleep();
}
