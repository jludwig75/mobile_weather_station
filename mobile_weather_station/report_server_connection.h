#pragma once

#include <WString.h>

#define DEFAULT_TIMEOUT 5

class GPRS;

class report_server
{
public:
  report_server();

  void turn_on_sim800l();
  void shutdown_sim800l();

  bool start_gprs_connection(int retries);

  int connectTCP(const String & server, uint16_t port, int retries);
  int sendTCPData(const String & data, int retries);
  int reportTempData(const String & host, unsigned short port, float temperature, float humiditiy, int retries);
  void disconnectTCP();

  void serialDebug();

private:
  bool init_gprs(int retries);
  bool connnect_to_mobile_network(int retries);

  int send_http_get_request(const String & url);
  int send_http_post_request(const String & url, const String & urldata);
  int read_http_header(unsigned int & status, String & status_text, unsigned int & content_length);
  int read_http_content(unsigned int content_length, String & content);

  GPRS *_gprs;
};
