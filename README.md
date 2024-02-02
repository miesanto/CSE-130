# Assignment 1: Command-line Memory (1/31/24)

## Files

- memory.c
- Makefile
- README.md

## Goal

This project serves as a refresher for building software in ‘C’. In particular, the project will involve
a review of Linux system calls, buffering file I/O, memory management, and C-string parsing.

## Memory.c Overview

Memory.c is designed to read commands from standard input (`stdin`) and perform operations based on valid `get` and `set` commands.

## Valid Commands

### `get` Command

- **Behavior:** A valid get command is formatted as “get\n<location>\n” and indicates that memory should output the contents of the file named location. If there is a file named location in the current directory, then memory should write the contents of location to stdout and exit with a return code of 0.

### `set` Command

- **Behavior:**  valid set command is formatted as “set\n<location>\n<content_length>\n<contents>” and indicates that memory should assign content_length bytes from contents to a file named location in the current directory. If location is a valid filename, then memory should assign the content of location to content_length bytes from content (overwriting location’s current contents if applicable), and exit with return code 0. In general, memory should be permissive on input.

## Invalid Commands

If an invalid command is detected, the program should:
- Output "Invalid Command\n" to stderr.
- Exit with a return code of 1.

### Invalid Command Conditions

1. **Command Prefix:** The command does not start with either "get" or "set" (case-sensitive).

2. **Incomplete Command:** The command starts with "get" or "set," but `stdin` was closed before the user provided a location.

3. **Invalid Filename:** The command starts with "get" or "set," includes a location, but the location is not a valid filename.

4. **File Not Found:** For `get` commands, the file at the specified location does not exist.

5. **Extra Input in Get Command:** For `get` commands, there is additional input beyond the expected format.

## Additional Functionality and Limitations

- If memory detects any other error, the program should produce the text Operation Failed to stderr and exit with a return code of 1.
- Memory must be reasonably time efficient.
- Memory must be reasonably space efficient.
- Memory must be permissive on input.
- Memory must not leak any memory.
- Memory must not leak any file descriptors.
- Memory should not crash.
- Memory must be written using the ‘C’ programming language.
- Memory cannot use the following functions from the ‘C’ stdio.h library: fwrite, fread, variants of put, and get.
- Memory cannot use functions, like system(3), that execute external programs.

## Usage

To use the "memory" program, provide valid commands via stdin. The program will execute the specified operations and produce output accordingly.

## Notes to the Grader

Knowing that LLMs produce faulty code anyway, I often asked ChatGPT to write pseudocode in non-technical terms to help outline my functions' logic (namely handleSet()). I achieved this by pasting appropriate notes from the assignment PDF, which greatly improved my understanding of the project. I also used ChatGPT to help debug (e.g., help with reorganizing lines if they caused seg faults) and to add clarifying comments to my code. I did not attend any office hours/tutoring sections for this project but I imagine I will start showing up a lot as this quarter progresses!