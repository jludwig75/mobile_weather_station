#pragma once

#include <WString.h>

#define DEFAULT_TIMEOUT 5

class GPRS;

class report_server
{
public:
  report_server();

  void turn_on_sim800l();
  void shutdown_gprs();

  bool start_gprs_connection();

  int connectTCP(const String & server, uint16_t port);
  int sendTCPData(const String & data);
  int readBuffer(char* buffer,int count, unsigned int timeOut = DEFAULT_TIMEOUT, bool raw = false);
  int readLine(char* buffer,int count, unsigned int timeOut = DEFAULT_TIMEOUT);
  int http_get(const String & host, unsigned short port, const String & url, unsigned int & status, String & status_text, String &response);
  void disconnectTCP();

  void serialDebug();
  
private:
  bool init_gprs(int retries);
  bool connnect_to_mobile_network(int retries);

  int send_http_get_request(const String & url);
  int read_http_header(unsigned int & status, String & status_text, unsigned int & content_length);
  int read_http_content(unsigned int content_length, String & content);

  GPRS *_gprs;
};
