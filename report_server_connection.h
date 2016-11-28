#pragma once

#include <gprs.h>

void turn_on_sim800l();

bool init_gprs(GPRS & gprs, int retries);

bool connnect_to_mobile_network(GPRS & gprs, int retries);

bool start_gprs_connection(GPRS & gprs);

void shutdown_gprs(GPRS & gprs);
