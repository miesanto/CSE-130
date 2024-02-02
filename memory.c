//-----------------------------------------------------------------------------
// Mia Santos (miesanto)
// CSE 130 Spring 2024 pa1
// memory.c
// Input: Commands ("get" or "set") from stdin
// Output: File contents or error messages to stdout/stderr.
//-----------------------------------------------------------------------------

// headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // for read and write functions
#include <fcntl.h> // for open function and file descriptors flags
#include <linux/limits.h> // for PATH_MAX
#include <sys/stat.h>

// macros
#define MAX_BUFF     4096 // buffer size for read and write
#define COMMAND_SIZE 4 // max length for a command string
#define O_FLAGS      (O_WRONLY | O_CREAT | O_TRUNC)

//-----------------------------------------------------------------------------
// error messages
//-----------------------------------------------------------------------------

// invalidCommand()
// an invalid command is one that meets any of the following:
//      the command does not start with either get or set (these are case-sensitive)
//      stdin was closed before the user provided a location
//      the location is not a valid filename
//      there is no file located at location
//      the command includes additional input
void invalidCommand(void) {
    dprintf(STDERR_FILENO, "Invalid Command\n");
    exit(1);
}

// operationFailed()
// if memory detects any other error not listed in invalidCommand
void operationFailed(int fd) {
    dprintf(STDERR_FILENO, "Operation Failed\n");
    close(fd);
    exit(1);
}

//-----------------------------------------------------------------------------
// helper functions
//-----------------------------------------------------------------------------

// openFileForReading()
// params:
//      filename: the name of the file to be opened
// returns:
//      file descriptor (int) if the file is successfully opened
//      exits the program with an error message if the file cannot be opened
int openFileForReading(const char *filename) {
    int fd = open(filename, O_RDONLY); // attempt to open the file in read-only mode

    if (fd == -1) {
        invalidCommand();
    }

    return fd;
}

// openFileForWriting()
// params:
//      filename: the name of the file to be opened or created
// returns:
//      file descriptor (int) if the file is successfully opened or created
//      exits the program with an error message if the file cannot be opened or created
int openFileForWriting(const char *filename) {
    int fd = open(
        filename, O_FLAGS, 0644); // attempt to open or create file with write-only permissions

    if (fd == -1) {
        invalidCommand();
    }

    return fd;
}

// retrieveContent()
// retrieve content from a file and write it to STDOUT
// params:
//      location: The name of the file whose content needs to be retrieved
// Returns:
//      1 if the content retrieval is successful
//      exits the program with an error message if the file cannot be opened or is a directory
int retrieveContent(const char *location) {
    int fd = openFileForReading(location); // open file for reading
    char buf[MAX_BUFF]; // character array storing data read with size MAX_BUFF
    ssize_t bytes_read; // store number of bytes read by 'read'

    // check for open error
    if (fd == -1) {
        invalidCommand();
    }

    // check if location is a directory
    struct stat filename_stat;
    stat(location, &filename_stat);
    if (S_ISDIR(filename_stat.st_mode)) {
        invalidCommand();
    }

    // continues as long as there are bytes to be read
    while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
        ssize_t bytes_written = write(STDOUT_FILENO, buf, bytes_read); // write to STDOUT

        // if write to STDOUT fails, report operation failure
        if (bytes_written == -1) {
            operationFailed(fd);
        }
    }

    close(fd);
    return 1; // success
}

//-----------------------------------------------------------------------------
// get and set commands
//-----------------------------------------------------------------------------

// handleGet()
// handle "get" command by reading locations from stdin and retrieving content
// assumes that locations are provided one per line, with newline characters as separators
// params:
//      none
// returns:
//      none
void handleGet(void) {
    char location[PATH_MAX + 1];

    // initialize location array with null characters
    for (int i = 0; i <= PATH_MAX; i++) {
        location[i] = '\0';
    }

    char *read_to = location; // current position in the location buffer to read into
    size_t remain_read = PATH_MAX + 1; // remaining number of bytes that can be read
    size_t total_read = 0; // total number of bytes read so far into location
    ssize_t bytes_read; // number of bytes read in the latest read operation

    // read input until newline or EOF (assuming read was successful)
    while ((bytes_read = read(STDIN_FILENO, read_to, remain_read)) > 0) {
        total_read += bytes_read; // update total_read with number of bytes read
        remain_read -= bytes_read;

        // increment read_to if there are more bytes to be read
        if (remain_read != 0) {
            read_to += bytes_read;
        }

        char *my_newline = strchr(location, '\n'); // find the position of the newline character

        // check if newline character is at the expected position
        if (my_newline != (location + total_read - 1)) {
            invalidCommand();
        }

        *my_newline = '\0'; // replace newline character with null character

        retrieveContent(location); // write to STDOUT
    }

    // check for read error
    if (bytes_read == -1) {
        invalidCommand();
    }
}

// handleSet()
// handle "set" command by reading locations from stdin and writing it to a file
// params:
//      none
// returns:
//      none
void handleSet(void) {
    char content[PATH_MAX + 1]; // buffer to store content read from stdin
    char *read_to
        = content; // pointer indicating the current position in the content buffer to read into
    char location[PATH_MAX + 1]; // buffer to store the location where the content will be written
    size_t remain_read = PATH_MAX + 1; // remaining number of bytes that can be read
    size_t total_read = 0; // total number of bytes read so far into the content
    ssize_t bytes_read; // number of bytes read in the latest read operation

    // read until newline or EOF (assuming read was successful)
    while ((bytes_read = read(STDIN_FILENO, read_to, remain_read)) > 0) {
        total_read += bytes_read;
        remain_read -= bytes_read;

        // increment remain_read if there are more bytes to be read
        if (remain_read != 0) {
            read_to += bytes_read;
        } else {
            break;
        }
    }

    // check for read error
    if (bytes_read == -1) {
        invalidCommand();
    }

    size_t total_remain = total_read; // total remaining bytes to process after reading the content
    char *c = content; // pointer to the beginning of content
    char *first_newline = strchr(content, '\n'); // pointer to the first newline char in content
    char *second_newline
        = strchr(first_newline + 1, '\n'); // pointer to the second newline char in content
    size_t len_location
        = first_newline
          - c; // length of the location string, calculated based on the position of first_newline

    total_remain -= len_location;

    // copy the location from the content buffer
    strncpy(location, c, len_location);
    location[len_location] = '\0'; // null-terminate the location string

    len_location = second_newline - first_newline;

    total_remain -= len_location;

    char data_str[PATH_MAX + 1];

    // copy the data length from the content buffer
    strncpy(data_str, first_newline, len_location);
    data_str[len_location] = '\0'; // null-terminate the data length string

    char *endptr;
    unsigned long d = strtoul(data_str, &endptr, 10);

    // check for conversion errors
    if (*endptr != '\0') {
        invalidCommand();
    }

    int fd = openFileForWriting(location); // open the file for writing
    size_t to_write = 0;

    // determine the initial number of bytes to write
    if ((total_remain - 1) > d) {
        to_write = d;
    } else {
        to_write = total_remain - 1;
    }

    // write the initial portion of the data to the file
    ssize_t bytes_written1 = write(fd, second_newline + 1, to_write);
    if (bytes_written1 == -1) {
        operationFailed(fd);
    }

    d -= to_write; // update remaining data length

    char buf[MAX_BUFF]; // buffer to store additional data read from STDIN
    ssize_t bytes_read2;

    // continue reading addl. data from STDIN and writing it to the file while d > 0
    while (d > 0) {
        bytes_read2 = read(STDIN_FILENO, buf, MAX_BUFF);

        if (bytes_read2 == -1) {
            invalidCommand(); // read error
        } else if (bytes_read2 == 0) {
            break; // EOF
        } else {
            to_write = bytes_read2;

            // write the additional data to the file
            ssize_t bytes_written2 = write(fd, buf, to_write);

            // check for write error
            if (bytes_written2 == -1) {
                operationFailed(fd);
            }

            d -= to_write; // update remaining data length
        }
    }

    // close file
    close(fd);

    // write "OK" to STDOUT_FILENO
    ssize_t bytes_written3 = write(STDOUT_FILENO, "OK\n", 3);
    if (bytes_written3 == -1) {
        operationFailed(fd);
    }
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

int main(void) {
    char buf[MAX_BUFF] = { '\0' }; // store the command read from STDIN
    char *read_to = buf; // pointer indicating the current position in buf to read into
    size_t remain_read = COMMAND_SIZE; // remaining number of bytes that can be read into buf

    while (1) {

        ssize_t bytes_read;

        // read input from STDIN into buf
        bytes_read = read(STDIN_FILENO, read_to, remain_read);
        if (bytes_read == -1 || bytes_read == 0) {
            invalidCommand(); // read error or EOF
        } else {
            remain_read -= bytes_read;

            if (remain_read != 0) {
                read_to += bytes_read; // increment if there are more bytes to be read
                continue;
            } else {
                break; // break when command is fully read
            }
        }
    }

    // check the command and call corresponding handler function
    if ((strncmp(buf, "get\n", COMMAND_SIZE) == 0) || (strncmp(buf, "get", COMMAND_SIZE) == 0)) {
        handleGet();
    } else if ((strncmp(buf, "set\n", COMMAND_SIZE) == 0)
               || (strncmp(buf, "set", COMMAND_SIZE) == 0)) {
        handleSet();
    } else {
        invalidCommand();
    }

    // success!
}
