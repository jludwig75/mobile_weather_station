#include "report_server_connection.h"

void turn_on_sim800l()
{
  digitalWrite(13, HIGH);
}

bool init_gprs(GPRS & gprs, int retries)
{
  gprs.preInit();
  
  for(int i = 0; i < retries && 0 != gprs.init(); i++)
  {
     delay(1000);
  }

  return gprs.init() == 0;
}

bool connnect_to_mobile_network(GPRS & gprs, int retries)
{
  for(int i = 0; i < retries; i++)
  {
    if (gprs.join("data.lycamobile.com")) //change "cmnet" to your own APN
    {
      // successful DHCP
      Serial.print("Successfully connected to mobile network.\nIP Address is ");
      Serial.println(gprs.getIPAddress());
      return true;
    }
    Serial.println("gprs join network error");
    delay(2000);
  }

  return false;
}

bool start_gprs_connection(GPRS & gprs)
{
  Serial.println("GPRS - HTTP Connection Test...");
  if (!init_gprs(gprs, 20))
  {
     Serial.println("GPRS init error");
     return false;
  }

  if (!connnect_to_mobile_network(gprs, 10))
  {
     Serial.println("Failed to connect to mobile network.");
     return false;
  }
  
  return true;
}

void shutdown_gprs(GPRS & gprs)
{
  Serial.println("Shutting down SIM800L");
  if (0 != gprs.sendCmdAndWaitForResp("AT+CPOWD=1", "OK", 3))
  {
    Serial.println("Failed to shutdown SIM800L");
  }

  // Cut the power
  digitalWrite(13, LOW);
}


