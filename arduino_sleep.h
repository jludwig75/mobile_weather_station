#pragma once


void configure_sleep(void (*on_wake)(), long sleep_time_seconds);

void sleep();

void manage_sleep();

