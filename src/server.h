//
// Created by dmytro on 8/29/23.
//

#ifndef TINYTIM_SERVER_H
#define TINYTIM_SERVER_H

#endif //TINYTIM_SERVER_H

#define BUFFER_SIZE 2048

const char *head_200 = "HTTP/1.1 200 OK\r\nServer: TinyTim\r\nContent-Type: text/html\r\n\r\n";
const char *head_404 = "HTTP/1.1 404 Not Found\r\nServer: TinyTim\r\nContent-Type: text/html\r\n\r\n";


char LOG_BUFFER[BUFFER_SIZE];
char BUFFER[BUFFER_SIZE];
char *ROOT = TINYTIM_ROOT;


void write_to_socket(int client_socket, const char * buffer) {
    write(client_socket, buffer, strlen(buffer));
}


void send_file(int client_socket, const char * location) {
    FILE *location_stream;
    if ((location_stream = fopen(location, "r")) != NULL) {

        sprintf(
                LOG_BUFFER,
                "Sending Body: %s",
                location
        );
        log_info(LOG_BUFFER);

        char *RESPONSE_BODY_BUFFER;

        RESPONSE_BODY_BUFFER = malloc(sizeof(char) * BUFFER_SIZE);
        size_t size = BUFFER_SIZE;

        while (getline(&RESPONSE_BODY_BUFFER, &size, location_stream) > 0) {
            write_to_socket(client_socket, RESPONSE_BODY_BUFFER);
            RESPONSE_BODY_BUFFER = NULL;
        }
    }
}


void *client_handler(void *client_socket_ptr) {
    int client_socket = *((int*)client_socket_ptr);
    free(client_socket_ptr);

    /* Read request from client */
    read(client_socket, BUFFER, BUFFER_SIZE);

    /*  Parse request */
    char *request_line = strtok(BUFFER, "\r\n");

    sprintf(
            LOG_BUFFER,
            "Received request: %s",
            request_line
    );
    log_info(LOG_BUFFER);

    char location[512] = {};
    sscanf(request_line, "GET /%[^ ]", location);

    if (strcmp(location, "") == 0) {

        /* send 200 OK Head */

        write_to_socket(client_socket, head_200);

        sprintf(
                LOG_BUFFER,
                "Sending Headers:\n%s",
                head_200
        );
        log_info(LOG_BUFFER);

        sprintf(
                location,
                "%sindex.html",
                ROOT
        );

        send_file(client_socket, location);

        /* Close connection */
        close(client_socket);
        return NULL;
    }

    char *tmp = strdup(location);
    strcpy(location, ROOT);
    strcat(location, tmp);
    free(tmp);

    /* Check if file exists */
    FILE *location_stream;
    if((location_stream = fopen(location, "r")) == NULL) {
        sprintf(
                LOG_BUFFER,
                "Can not open location: %s",
                location
                );
        log_warning(LOG_BUFFER);

        /* send 404 NOT FOUND Head */

        write_to_socket(client_socket, head_404);

        sprintf(
                LOG_BUFFER,
                "Sending Headers:\n%s",
                head_404
        );
        log_info(LOG_BUFFER);

        sprintf(
                location,
                "%s404.html",
                ROOT
                );

        send_file(client_socket, location);

        /* Close connection */
        close(client_socket);
        return NULL;

    }
    else {
        sprintf(
                LOG_BUFFER,
                "Openning location: %s",
                location
                );
        log_info(LOG_BUFFER);
    }

    /* send 200 OK Head */

    write_to_socket(client_socket, head_200);

    sprintf(
            LOG_BUFFER,
            "Sending Head:\n%s",
            head_200
    );
    log_info(LOG_BUFFER);

    /* Send body */
    send_file(client_socket, location);

    /* Close connection */
    close(client_socket);
    return NULL;

}
