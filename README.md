# Project Overview

This project consists of multiple C programs and header files designed for user management, server administration, and communication utilities. Below is an overview of each file and its functionality.

## File Descriptions

### `addUser.c`
This file is a simple program for adding a user.
- Includes standard input/output, string manipulation, and standard library headers, along with a custom header `clientUtils.h`.
- **Main function**:
  - Checks if a username is provided as a command-line argument. If not, it prints usage instructions and exits.
  - Copies the provided username into a local buffer `username` while ensuring it does not exceed the buffer size.
  - Calls the `add_user` function (defined in `clientUtils.c`) to add the user and prints a success or failure message based on the return value of `add_user`.

### `admin.c`
This file is an admin utility for performing backup and restore operations.
- Includes standard input/output, string manipulation, and standard library headers, along with a custom header `servUtils.h`.
- **Main function**:
  - Checks if a command is provided as a command-line argument. If not, it prints usage instructions and exits.
  - Compares the provided command to "backup" or "restore".
  - Calls `perform_backup` or `perform_restore` functions (defined in `servUtils.c`) based on the command and prints a success or failure message based on the return value of these functions.

### `backup.c`
This file is a dedicated utility for performing backups.
- Includes standard input/output and standard library headers, along with a custom header `servUtils.h`.
- **Main function**:
  - Calls the `perform_backup` function (defined in `servUtils.c`) and prints a success or failure message based on the return value of the function.

### `clientUtils.c`
This file provides an implementation for the `add_user` function used in `addUser.c`.
- Includes standard input/output, string manipulation, and standard library headers, along with a custom header `clientUtils.h`.
- **`add_user` function**:
  - Takes a `username` as input.
  - Prints a message indicating the user being added.
  - Simulates successful user addition by returning 0.

### `clientUtils.h`
This header file declares the `add_user` function used in `addUser.c`.

### `comm.h`
This header file declares functions related to communication, likely used in other parts of the project.
- Functions declared:
  - `void send_message(const char *message);`
  - `void receive_message(char *buffer, size_t length);`

### `serv_init.c`
This file is a utility for initializing a server.
- Includes standard input/output and standard library headers, along with a custom header `servUtils.h`.
- **Main function**:
  - Calls the `initialize_server` function (defined in `servUtils.c`) and prints a success or failure message based on the return value of the function.

### `servUtils.c`
This file provides implementations for the `perform_backup`, `perform_restore`, and `initialize_server` functions used in other parts of the project.
- Includes standard input/output and standard library headers, along with a custom header `servUtils.h`.
- **`perform_backup` function**:
  - Prints a message indicating that a backup is being performed.
  - Simulates successful backup by returning 0.
- **`perform_restore` function**:
  - Prints a message indicating that a restore is being performed.
  - Simulates successful restore by returning 0.
- **`initialize_server` function**:
  - Prints a message indicating that the server is being initialized.
  - Simulates successful initialization by returning 0.

### `servUtils.h`
This header file declares the `perform_backup`, `perform_restore`, and `initialize_server` functions used in other parts of the project.

### `user.c`
This file is a simple program for deleting a user.
- Includes standard input/output, string manipulation, and standard library headers, along with a custom header `clientUtils.h`.
- **Main function**:
  - Checks if a username is provided as a command-line argument. If not, it prints usage instructions and exits.
  - Copies the provided username into a local buffer `username` while ensuring it does not exceed the buffer size.
  - Calls the `delete_user` function (defined in `clientUtils.c`) to delete the user and prints a success or failure message based on the return value of `delete_user`.

For more details check report.
