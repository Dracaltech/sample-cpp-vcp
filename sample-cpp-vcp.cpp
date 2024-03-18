#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "checksum.h" // CRC calculation library from github.com/lammertb/libcrc


// COM id of the plugged in device.
const char* dev = "\\\\.\\COM4";
const int line_size_max = 256;


// Small enum type for readline to return
typedef enum {
    SUCCESS,
    READ_ERROR,
    INTEGRITY_ERROR,
} error_t;

error_t read_line(HANDLE h, char* line) {

    memset(line, '\0', line_size_max);

    char buf[2]; // Buffer of 1 character + null terminator
    memset(buf, '\0', sizeof buf);

    do {
        if (!ReadFile(h, buf, 1, NULL, NULL)) {
            return READ_ERROR;
        }
        strcat_s(line, line_size_max, buf);
    } while (!strchr(line, '\n'));

    uint16_t crc;       // Checksum value read from the string
    char* sep;          // Position of the asterisk in the line

    sep = strchr(line, '*');
    if (!sep) {
        return INTEGRITY_ERROR;
    }
    crc = (uint16_t)strtol(sep + 1, NULL, 16);

    if (crc != crc_xmodem((unsigned char*)line, (size_t)(sep - line))) {
        return INTEGRITY_ERROR;
    }
    *sep = '\0'; // Replace the * with a null character, now line stops at the end of the content
    return SUCCESS;
}

int main()
{
    HANDLE COM = CreateFileA(dev,           // Port name
        GENERIC_READ | GENERIC_WRITE,       // Read/Write
        0,                                  // No Sharing
        NULL,                               // No Security
        OPEN_EXISTING,                      // Open existing port only
        0,                                  // Non Overlapped I/O
        NULL);                              // Null for Comm Devices

    if (COM == INVALID_HANDLE_VALUE) {
        printf("Error opening serial port\r\n");
        return EXIT_FAILURE;
    }
    else {
        printf("Opening serial port successful\r\n");
    }


    char line[256];
    memset(line, '\0', sizeof line);

    DWORD comm_mask;

    GetCommMask(COM, &comm_mask);
    printf("comm_mask: %x\r\n", comm_mask);

    // Variables for timekeeping
    time_t t;
    struct tm localt;
    char timestr[20];

    // Variables containing processed data from the device. This example was written with a PTH in mind
    int    pressure;      // Pressure in Pascals
    float  temperature;   // Temperature in Celsius
    float  humidity;      // Humidity in %
    char   model[32] = "";     // Model id of device
    char   serial[7] = "";     // Serial number of device
    char   message[128];  // Message contained for info lines

    char poll_cmd[] = "POLL 1000\r\n";

    // Whether an info line has been read yet
    bool info_line_read = false;

    for (int i = 0;; i++) {

        if (!info_line_read) {
            //PurgeComm(COM, PURGE_TXCLEAR);
            if (!WriteFile(COM, poll_cmd, sizeof poll_cmd - 1, NULL, NULL)) {
                printf("Write error = %i", GetLastError());
            }
        }

        switch (read_line(COM, line)) {
        case SUCCESS: // Here, the code that runs when everything is fine


            if (line[0] == 'I') {
                // For info lines (the POLL response in this case)
                sscanf_s(
                    line,
                    "I,%[^,],%[^,],%[^,]",
                    model, (int)sizeof model,
                    serial, (int)sizeof serial,
                    message, (int)sizeof message
                );

                printf("\n%s\n", message);
                info_line_read = true;
            }
            else {
                t = time(NULL);
                localtime_s(&localt, &t);
                strftime(timestr, 20, "%F %T", &localt); // YYYY-MM-DD HH:MM:SS

                /**
                 * Interpret the line and save the result into the variables.
                 * The format string to use would depend on the sensor, this example was made with the PTH sensor in mind.
                 * Refer to these resources to learn more on how to do so
                 *   Format strings for the scanf functions  : cplusplus.com/reference/cstdio/scanf/
                 *   Dracal sensor VCP mode output format    : dracal.com/en/usage-guides/vcp_howto
                **/
                sscanf_s(line, "%*c,%*[^,],%*[^,],,%i,%*2c,%f,%*c,%f", &pressure, &temperature, &humidity);

                // This is where you would put your code to be executed on data.
                printf(
                    "\n%s %s @ %s\nP = %i Pa\nT = %.2f C\nH = %.2f %%\n",
                    model, serial, timestr,
                    pressure, temperature, humidity
                );
            }

            break;

        case READ_ERROR: // This may happen if eg. the device is unplugged
            return EXIT_FAILURE;

        case INTEGRITY_ERROR: // When the integrity check failed
            if (i != 0) {
                // First line is likely to be garbage, no need to warn us about it
                printf("Integrity error on line %i: \"%s\"", i, line);
            }
            break;
        }
    }


    CloseHandle(COM); // Close the serial port

    return EXIT_SUCCESS;
}
