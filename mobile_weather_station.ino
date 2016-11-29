#include <Arduino.h>

#include <sim800.h>
#include <SoftwareSerial.h>

#include "arduino_sleep.h"
#include "report_server_connection.h"

// Data logging configuration.
#define LOGGING_FREQ_SECONDS   300       // Seconds to wait before a new sensor reading is logged.


char http_url[] = "/wiki/images/1/15/Hello.txt";

report_server report;

unsigned long sketch_start_time = 0;

void report_temperature()
{
  unsigned long elapsed = millis() - sketch_start_time;
  elapsed /= 1000;
  Serial.print("Reporting temperature at ");
  Serial.print(elapsed);
  Serial.println(" seconds");

  report.turn_on_sim800l();
  Serial.println("Turned on SIM800L");

  if (!report.start_gprs_connection())
  {
    Serial.println("Failed to connect to mobile network,");
    return;
  }

  int ret = report.reportTempData("54.214.48.0", 8888, 72, 45);
  if (0 != ret)
  {
    Serial.println("Failed to post data");
  }

  Serial.println("Shutting down GPRS");
  report.shutdown_gprs();
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Serial.println("Mobile Weather Station");

  sketch_start_time = 0;
  report_temperature();

  configure_sleep(report_temperature, LOGGING_FREQ_SECONDS);
}

void loop()
{
  manage_sleep();
}
