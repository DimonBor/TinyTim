//
// Created by dmytro on 8/29/23.
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <libconfig.h>
#include <sys/socket.h>
#include <netinet/in.h>


#include "main.h"
#include "logger.h"
#include "server.h"


int main (int argc, char *argv[]) {

    /* Configuration */
    int PORT = TINYTIM_PORT;
    char *CONF_FILE_NAME = TINYTIM_CONF_FILE_NAME;

    /* Temporary vars */
    config_t cfg;
    int TMP_INT;
    const char *TMP_STRING;
    char *LOG_BUFFER = malloc(sizeof(char)*2048);

    log_info(
            "Starting TinyTim HTTP server..."
           );

    /* Checking configuration */

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--config") || !strcmp(argv[i], "-c" )) {
            if (argc - i > 1) {
                CONF_FILE_NAME = malloc(strlen(argv[i + 1]) + 1);
                strcpy(CONF_FILE_NAME, argv[i + 1]);
            }
        }
    }

    config_init(&cfg);

    /* Check if we can read the file */
    if (!config_read_file(&cfg, CONF_FILE_NAME)) {
        sprintf(
                LOG_BUFFER,
                "Can not load config file %s:%d - %s",
                config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg)
                );
        log_warning(LOG_BUFFER);

        config_destroy(&cfg);
        CONF_FILE_NAME = "None";
    }
    else { /* Load values from config if possibe */
        if (config_lookup_int(&cfg, "port", &TMP_INT))
            PORT = TMP_INT;
        if (config_lookup_string(&cfg, "root", &TMP_STRING)) {
            ROOT = malloc(strlen(TMP_STRING)+1);
            strcpy(ROOT, TMP_STRING);
        }
        config_destroy(&cfg);
    }

    /* Overriding config params with cli params */
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--root") || !strcmp(argv[i], "-r")) {
            if (argc - i > 1) {
                ROOT = malloc(strlen(argv[i + 1]) + 1);
                strcpy(ROOT, argv[i + 1]);
            }
        }
        if (!strcmp(argv[i], "--port") || !strcmp(argv[i], "-p")) {
            if (argc - i > 1) {
                PORT = atoi(argv[i + 1]);
            }
        }
    }

    /* Configuration summary */
    sprintf(
            LOG_BUFFER,
            "Current configuration:\n\tUsing config file: %s\n\tPort: %d\n\tRoot: %s\n",
            CONF_FILE_NAME, PORT, ROOT
            );
    log_info(LOG_BUFFER);

    /* Starting server */

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;

    socklen_t client_addr_len = sizeof(client_addr);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    retry :

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
        log_error("Failed to bind port");
        log_warning("Retrying...");
        sleep(1);
        goto retry;
    }

    listen(server_socket, 10);

    sprintf(
            LOG_BUFFER,
            "Successfully bind to port %d",
            PORT
    );
    log_info(LOG_BUFFER);

    while (1) {

        client_socket = accept(
                server_socket,
                (struct sockaddr *)&client_addr,
                &client_addr_len
                );

        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        pthread_t client_thread;

        pthread_create(&client_thread, NULL, client_handler, client_socket_ptr);
        pthread_detach(client_thread);

    }

}