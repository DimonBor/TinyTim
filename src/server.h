//
// Created by dmytro on 8/29/23.
//

#ifndef TINYTIM_SERVER_H
#define TINYTIM_SERVER_H

#endif //TINYTIM_SERVER_H

#define BUFFER_SIZE 2048
#define RESPONSE_BODY_BUFFER_SIZE 2048


const char *head_200 = "HTTP/1.1 200 OK\r\nServer: TinyTim\r\n";
const char *head_404 = "HTTP/1.1 404 Not Found\r\nServer: TinyTim\r\n";

char *ROOT = TINYTIM_ROOT;
char *TRY_FILES = TINYTIM_TRY_FILES;


char * strrev(char * str) {
    if (!str || !*str)
        return str;

    for (int left = 0, right = strlen(str) - 1; left < right; left++, right--) {
        char tempChar = str[left];
        str[left] = str[right];
        str[right] = tempChar;
    }

    return str;
}


char *find_file() {
    char * files = malloc(strlen(TRY_FILES));
    strcpy(files, TRY_FILES);
    char * try_file = strtok(files, " ");
    char * try_file_full_path = malloc(sizeof(char) * BUFFER_SIZE);
    sprintf(try_file_full_path, "%s%s", ROOT, try_file);

    while (try_file != NULL) {
        FILE *location_stream;
        if((location_stream = fopen(try_file_full_path, "r")) != NULL) return try_file_full_path;
        try_file = strtok(NULL, " ");
        sprintf(try_file_full_path, "%s%s", ROOT, try_file);
    }

    return NULL;
}


void write_to_socket(int client_socket, const char * buffer, int buffer_size) {
    write(client_socket, buffer, buffer_size);
}


void send_file(int client_socket, const char * location) {
    char LOG_BUFFER[BUFFER_SIZE];
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
            int fetched_size;
            char RESPONSE_BODY_BUFFER[RESPONSE_BODY_BUFFER_SIZE] = {0};
            while ((fetched_size = fread(RESPONSE_BODY_BUFFER, sizeof(char), RESPONSE_BODY_BUFFER_SIZE, location_stream)) == RESPONSE_BODY_BUFFER_SIZE) {
                write_to_socket(client_socket, RESPONSE_BODY_BUFFER, RESPONSE_BODY_BUFFER_SIZE);
            }
            if (fetched_size > 0 && fetched_size < RESPONSE_BODY_BUFFER_SIZE) {
                write_to_socket(client_socket, RESPONSE_BODY_BUFFER, fetched_size);
            }
        }
    }
}


const char *get_content_type(char * location) {
    char extension[10] = {};
    sscanf(strrev(location), "%[^.].%*[]", extension);
    strrev(extension);
    strrev(location);

    if (!strcmp(extension, "html"))
        return "Content-Type: text/html\r\n\r\n";
    if (!strcmp(extension, "js"))
        return "Content-Type: text/javascript\r\n\r\n";
    if (!strcmp(extension, "json"))
        return "Content-Type: application/json\r\n\r\n";
    if (!strcmp(extension, "png"))
        return "Content-Type: image/png\r\n\r\n";
    if (!strcmp(extension, "jpg"))
        return "Content-Type: image/jpeg\r\n\r\n";
    if (!strcmp(extension, "svg"))
        return "Content-Type: image/svg+xml\r\n\r\n";
    if (!strcmp(extension, "css"))
        return "Content-Type: text/css\r\n\r\n";
    if (!strcmp(extension, "txt"))
        return "Content-Type: text/plain\r\n\r\n";
    return "Content-Type: application/octet-stream\r\n\r\n";
}


void *client_handler(void *client_socket_ptr) {
    char BUFFER[BUFFER_SIZE];
    char LOG_BUFFER[BUFFER_SIZE];

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
    sscanf(request_line, "GET /%[^ ?][ ?]%*[]", location);

    char *tmp = strdup(location);
    strcpy(location, ROOT);
    strcat(location, tmp);
    free(tmp);

    /* Check if file exists */
    FILE *location_stream;

    if((location_stream = fopen(location, "r+")) == NULL) {
        sprintf(
                LOG_BUFFER,
                "Can not open file: \"%s\"...",
                location
                );
        log_warning(LOG_BUFFER);

        char location[512] = {};
        char * try_file;

        if ((try_file = find_file()) != NULL) {

            const char * content_type = get_content_type(try_file);
            char * head_200_type = malloc(strlen(head_200) + strlen(content_type));

            strcpy(head_200_type, head_200);
            strcat(head_200_type, content_type);

            sprintf(
                    LOG_BUFFER,
                    "Sending Headers:\n%s",
                    head_200_type
                    );
            log_info(LOG_BUFFER);

            write_to_socket(client_socket, head_200_type, strlen(head_200_type));
            send_file(client_socket, try_file);

            /* Close connection */
            close(client_socket);
            return NULL;

        }
        else {
            /* send 404 NOT FOUND Head */

            sprintf(
                    location,
                    "%s404.html",
                    ROOT
                    );

            const char * content_type = get_content_type(location);
            char * head_404_type = malloc(strlen(head_404) + strlen(content_type));

            strcpy(head_404_type, head_404);
            strcat(head_404_type, content_type);

            sprintf(
                    LOG_BUFFER,
                    "Sending Headers:\n%s",
                    head_404_type
                    );
            log_info(LOG_BUFFER);

            write_to_socket(client_socket, head_404_type, strlen(head_404_type));
            send_file(client_socket, location);

            /* Close connection */
            close(client_socket);
            return NULL;
        }


    }
    else {
        sprintf(
                LOG_BUFFER,
                "Openning location: %s",
                location
                );
        log_info(LOG_BUFFER);
    }

    /* Sending exact file, send 200 OK Head */

    const char * content_type = get_content_type(location);
    char * head_200_type = malloc(strlen(head_200) + strlen(content_type));

    strcpy(head_200_type, head_200);
    strcat(head_200_type, content_type);

    sprintf(
            LOG_BUFFER,
            "Sending Headers:\n%s",
            head_200_type
    );
    log_info(LOG_BUFFER);

    write_to_socket(client_socket, head_200_type, strlen(head_200_type));
    send_file(client_socket, location);

    /* Close connection */
    close(client_socket);
    return NULL;

}
