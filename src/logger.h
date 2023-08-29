//
// Created by dmytro on 8/29/23.
//

#ifndef TINYTIM_LOGGER_H
#define TINYTIM_LOGGER_H

#endif //TINYTIM_LOGGER_H

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#include <time.h>
#include <string.h>

void log_info (const char * message) {
    time_t now;
    time(&now);
    printf(
            ANSI_COLOR_GREEN "%s [INFO]: %s\n" ANSI_COLOR_RESET,
            strtok(ctime(&now), "\n"), message
            );
}


void log_warning (const char * message) {
    time_t now;
    time(&now);
    printf(
            ANSI_COLOR_YELLOW "%s [WARNING]: %s\n" ANSI_COLOR_RESET,
            strtok(ctime(&now), "\n"), message
            );
}


void log_error (const char * message) {
    time_t now;
    time(&now);
    printf(
            ANSI_COLOR_RED "%s [ERROR]: %s\n" ANSI_COLOR_RESET,
            strtok(ctime(&now), "\n"), message
            );
}