//
// Created by dmytro on 8/29/23.
//

#ifndef TINYTIM_SERVER_H
#define TINYTIM_SERVER_H

#endif //TINYTIM_SERVER_H

#define BUFFER_SIZE 2048

const char *head_200 = "HTTP/1.1 200 OK\r\nServer: TinyTim\r\n\r\n";
const char *head_404 = "HTTP/1.1 404 Not Found\r\nServer: TinyTim\r\n\r\n";


char LOG_BUFFER[BUFFER_SIZE];
char BUFFER[BUFFER_SIZE];
char *ROOT = TINYTIM_ROOT;


void write_to_socket(int client_socket, const char * buffer, int buffer_size) {
    write(client_socket, buffer, buffer_size);
}


void send_file(int client_socket, const char * location) {
    FILE *location_stream;
    if ((location_stream = fopen(location, "rb")) != NULL) {

        sprintf(
                LOG_BUFFER,
                "Sending Body: %s",
                location
        );
        log_info(LOG_BUFFER);

        fseek(location_stream, 0, SEEK_END);
        size_t file_size = ftell(location_stream);
        rewind(location_stream);

        if (file_size < BUFFER_SIZE) {
            char RESPONSE_BODY_BUFFER[file_size] = {};
            fread(RESPONSE_BODY_BUFFER, sizeof(char), file_size, location_stream);
            write_to_socket(client_socket, RESPONSE_BODY_BUFFER, file_size);
        }
        else {
            char RESPONSE_BODY_BUFFER[BUFFER_SIZE] = {0};
            while (fread(RESPONSE_BODY_BUFFER, sizeof(char), BUFFER_SIZE, location_stream) == BUFFER_SIZE) {
                write_to_socket(client_socket, RESPONSE_BODY_BUFFER, BUFFER_SIZE);
            }
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

        write_to_socket(client_socket, head_200, strlen(head_200));

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

        write_to_socket(client_socket, head_404, strlen(head_404));

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

    write_to_socket(client_socket, head_200, strlen(head_200));

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
