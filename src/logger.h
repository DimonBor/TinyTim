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

void log_info (const char * message) {
    time_t now;
    pid_t tid;
    time(&now);
    tid = syscall(SYS_gettid);
    printf(
            ANSI_COLOR_GREEN "%s [%d] [INFO]: %s\n" ANSI_COLOR_RESET,
            strtok(ctime(&now), "\n"), tid, message
            );
}


void log_warning (const char * message) {
    time_t now;
    pid_t tid;
    time(&now);
    tid = syscall(SYS_gettid);
    printf(
            ANSI_COLOR_YELLOW "%s [%d] [WARNING]: %s\n" ANSI_COLOR_RESET,
            strtok(ctime(&now), "\n"), message
            );
}


void log_error (const char * message) {
    time_t now;
    pid_t tid;
    time(&now);
    tid = syscall(SYS_gettid);
    printf(
            ANSI_COLOR_RED "%s [%d] [ERROR]: %s\n" ANSI_COLOR_RESET,
            strtok(ctime(&now), "\n"), message
            );
}