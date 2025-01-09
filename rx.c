#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#define CANDUMP_CMD  "candump can0"  // Adjust the CAN interface as necessary

// Function to convert hex string to ASCII
void hex_to_ascii(const char *hex_buffer) {
    size_t len = strlen(hex_buffer);
    char *clean_hex = malloc(len + 1); // Allocate buffer with space for null-terminator
    if (clean_hex == NULL) {
        perror("malloc");
        return;
    }

    size_t clean_index = 0;
    for (size_t i = 0; i < len; ++i) {
        if (hex_buffer[i] != ' ') {
            clean_hex[clean_index++] = hex_buffer[i];
        }
    }
    clean_hex[clean_index] = '\0'; // Null-terminate the cleaned hex string

    len = clean_index; // Update length to reflect cleaned hex string length

    // Check if the length of clean_hex is even
    if (len % 2 != 0) {
        fprintf(stderr, "Hex buffer length must be even after cleaning.\n");
        free(clean_hex);
        return;
    }

    // Allocate buffer for the ASCII output
    char *ascii_buffer = malloc(len / 2 + 1); // +1 for null-terminator
    if (ascii_buffer == NULL) {
        perror("malloc");
        free(clean_hex);
        return;
    }

    // Convert hex string to ASCII string
    for (size_t i = 0; i < len; i += 2) {
        char hex_pair[3] = {clean_hex[i], clean_hex[i + 1], '\0'};
        unsigned int decimal_value;
        if (sscanf(hex_pair, "%2x", &decimal_value) != 1) {
            fprintf(stderr, "Failed to convert hex pair '%s' to decimal.\n", hex_pair);

            free(ascii_buffer);
            free(clean_hex);
            return;
        }
        ascii_buffer[i / 2] = (char)decimal_value;
    }

    // Null-terminate the ASCII string
    ascii_buffer[len / 2] = '\0';
    printf("ASCII Output: %s\n", ascii_buffer);

    free(ascii_buffer);
    free(clean_hex);
}

// Function to process each CAN message
void process_can_message(char *message) {
    unsigned int msg_id;
    int dlc;
    char msg_data[256];
    char *data_ptr;

    if (sscanf(message, " can0 %x   [%d] ", &msg_id, &dlc) != 2) {
        fprintf(stderr, "Failed to parse CAN message ID and DLC: %s\n", message);
        return;
    }

    data_ptr = strchr(message, ']');
    if (data_ptr == NULL) {
        fprintf(stderr, "Failed to find data part: %s\n", message);
        return;
    }
    data_ptr += 3; // Move past "] " to the start of the data

    strncpy(msg_data, data_ptr, sizeof(msg_data) - 1);
    msg_data[sizeof(msg_data) - 1] = '\0';

    char *end = msg_data + strlen(msg_data) - 1;
    while (end > msg_data && (*end == '\n' || *end == ' ')) {
        *end-- = '\0';
    }

    printf("Message ID: %X\n", msg_id);
    printf("Data Length: %d\n", dlc);
    printf("Message Data: %s\n", msg_data);

    hex_to_ascii(msg_data);  // Convert and print the hex data
}

// Function to listen for CAN messages
void *can_listener(void *arg) {
    FILE *fp;
    char line[256];

    fp = popen(CANDUMP_CMD, "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line) - 1, fp) != NULL) {
        process_can_message(line);  // Process each CAN message
    }

    pclose(fp);
    return NULL;
}

// Main function to start listening
int main() {
    pthread_t can_thread;
    if (pthread_create(&can_thread, NULL, can_listener, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    pthread_join(can_thread, NULL);  // Wait for the thread to finish
    return 0;
}
