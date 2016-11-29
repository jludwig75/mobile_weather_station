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
     delay(1000);
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
    delay(2000);
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
  return _gprs->sendTCPData(data.c_str());
}

int report_server::readBuffer(char* buffer,int count, unsigned int timeOut, bool raw)
{
  return _gprs->readBuffer(buffer, count, timeOut, raw);
}


int report_server::readLine(char* buffer,int count, unsigned int timeOut)
{
  return _gprs->readLine(buffer, count, timeOut);
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

int report_server::http_get(const String & host, unsigned short port, const String & url, unsigned int & status, String & status_text, String &response)
{
  int ret;

  ret = connectTCP(host, port);
  if (0 != ret)
  {
    Serial.println("Failed to connect to host");
    return ret;
  }
  
  ret = send_http_get_request(url);
  if (0 != ret)
  {
    Serial.println("Failed to send GET request");
    disconnectTCP();
    return ret;
  }

  unsigned int contentLength = 0;
  ret = read_http_header(status, status_text, contentLength);
  if (0 != ret)
  {
    Serial.println("Failed to read HTTP header");
    disconnectTCP();
    return ret;
  }

  if (contentLength > 0)
  {
    ret = read_http_content(contentLength, response);
    if (0 != ret)
    {
      Serial.println("Failed to read HTTP content");
      disconnectTCP();
      return ret;
    }
  }
  
  disconnectTCP();
  return 0;
}

int report_server::send_http_get_request(const String & url)
{
  int ret = sendTCPData("GET ");
  if(0 != ret)
  {
    return ret;
  }

  ret = sendTCPData(url.c_str());
  if(0 != ret)
  {
    return ret;
  }

  ret = sendTCPData(" HTTP/1.0\r\n\r\n");
  if(0 != ret)
  {
    return ret;
  }

  return 0;
}

int report_server::read_http_header(unsigned int & status, String & status_text, unsigned int & content_length)
{
  char line_buffer[128];
  int state = 0;
  content_length = 0;
  unsigned long end_time = millis() + 5000;
  while(millis() < end_time)
  {
    memset(line_buffer, 0, sizeof(line_buffer));
    int ret = readLine(line_buffer, sizeof(line_buffer) - 1);
    if (0 != ret)
    {
      return ret;
    }
    String line = line_buffer;

    switch(state)
    {
    case 0:
      {
        if (line.startsWith("HTTP"))
        {
          state = 1;
        }
      }
      break;
    case 1:
      {
        if (line.length() > 0)
        {
          if (line.startsWith("Content-Length"))
          {
            int colonPos = line.lastIndexOf(":");
            String contentLengthString = line.substring(colonPos + 1);
            contentLengthString .trim();
            content_length = contentLengthString.toInt();
          }
        }
        else
        {
          return 0;
        }
      }
      break;
    }
  }

  return -1;
}

int report_server::read_http_content(unsigned int content_length, String & content)
{
  if (content_length > 0)
  {
    char *content_buffer = new char[content_length + 1];
    if (!content_buffer)
    {
      Serial.print("Failed to allocate content buffer: ");
      Serial.print(content_length + 1);
      Serial.println(" bytes");
      return -1;
    }
    memset(content_buffer, 0, content_length + 1);
    readBuffer(content_buffer, content_length, 5, true);
    content = String(content_buffer);
    delete [] content_buffer;
  }
  
  return 0;
}

