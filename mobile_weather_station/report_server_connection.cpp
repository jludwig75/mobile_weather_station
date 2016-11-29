#include "report_server_connection.h"

#include <gprs.h>

report_server::report_server()
{
  _gprs = new GPRS;
}

void report_server::turn_on_sim800l()
{
  digitalWrite(13, HIGH);
}

bool report_server::init_gprs(int retries)
{
  _gprs->preInit();

  for(int i = 0; i < retries && 0 != _gprs->init(); i++)
  {
     delay(500);
  }

  return _gprs->init() == 0;
}

bool report_server::connnect_to_mobile_network(int retries)
{
  for(int i = 0; i < retries; i++)
  {
    if (_gprs->join("data.lycamobile.com")) //change "cmnet" to your own APN
    {
      // successful DHCP
      Serial.print("Successfully connected to mobile network.\nIP Address is ");
      Serial.println(_gprs->getIPAddress());
      return true;
    }
    if (i > 0 || i == retries - 1)
    {
      Serial.println("gprs join network error");
    }
    delay(1000);
  }

  return false;
}

bool report_server::start_gprs_connection()
{
  if (!init_gprs(20))
  {
     return false;
  }

  if (!connnect_to_mobile_network(10))
  {
     return false;
  }

  return true;
}

int report_server::connectTCP(const String & server, uint16_t port)
{
  return _gprs->connectTCP(server.c_str(), port);
}

int report_server::sendTCPData(const String & data)
{
  return _gprs->sendTCPData((char *)data.c_str());
}

void report_server::disconnectTCP()
{
      _gprs->closeTCP();
      _gprs->shutTCP();
}

void report_server::shutdown_gprs()
{
  Serial.println("Shutting down SIM800L");
  if (0 != _gprs->sendCmdAndWaitForResp("AT+CPOWD=1", "OK", 3))
  {
    Serial.println("Failed to shutdown SIM800L");
  }

  // Cut the power
  digitalWrite(13, LOW);
}

void report_server::serialDebug()
{
  _gprs->serialDebug();
}

int report_server::reportTempData(const String & host, unsigned short port, float temperature, float humiditiy)
{
  int ret;

  ret = connectTCP(host, port);
  if (0 != ret)
  {
    Serial.println("Failed to connect to host");
    return ret;
  }

  String message = String("{\"temp\": ") + String(temperature) + ", \"bat\": 3.3, \"signal\": 1, \"berror\": 0}";

  ret = sendTCPData(message.c_str());
  if(0 != ret)
  {
    Serial.println("Failed to send data to host");
    disconnectTCP();
    return ret;
  }

  disconnectTCP();
  return 0;
}
