#include <Arduino.h>

#include "arduino_sleep.h"
#include "report_server_connection.h"

#include <sim800.h>
#include <SoftwareSerial.h>

// Data logging configuration.
#define LOGGING_FREQ_SECONDS    (15 * 60)       // Seconds to wait before a new sensor reading is logged.
#define START_GPRS_RETRIES      3
#define REPORT_TEMP_RETRIES     3
#define MAX_RETRIES             3


char http_url[] = "/wiki/images/1/15/Hello.txt";

report_server report;

unsigned long sketch_start_time = 0;

bool report_temperature()
{
  unsigned long elapsed = millis() - sketch_start_time;
  elapsed /= 1000;
  Serial.print("Reporting temperature at ");
  Serial.print(elapsed);
  Serial.println(" seconds");

  if (!report.start_gprs_connection(START_GPRS_RETRIES))
  {
    Serial.println("Failed to connect to mobile network,");
    return false;
  }

  int ret = report.reportTempData("54.214.48.0", 8888, 72, 45, REPORT_TEMP_RETRIES);
  if (0 == ret)
  {
    Serial.println("Successfully reported weather data to server");
  }
  else
  {
    Serial.println("Failed to report weather data to server");
    return false;
  }

  return true;
}

void on_wake()
{
  for(int i = 0; i < MAX_RETRIES; i++)
  {
    Serial.println("Turning on SIM800L");
    report.turn_on_sim800l();

    bool success = report_temperature();

    Serial.println("Shutting down GPRS");
    report.shutdown_sim800l();

    if (success)
    {
      break;
    }

    // If it failed, and it's not the last attempt,
    if (i < MAX_RETRIES - 1)
    {
      // delay 3 seconds and try again.
      delay(3000);
      Serial.println("Retrying report data");
    }
  }

}

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Serial.println("Mobile Weather Station");

  sketch_start_time = 0;
  on_wake();

  configure_sleep(on_wake, LOGGING_FREQ_SECONDS);
}

void loop()
{
  manage_sleep();
}
