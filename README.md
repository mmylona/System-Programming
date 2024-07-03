# README

## Overview

This repository contains two main projects developed as part of the System Programming course in the Department of Informatics and Telecommunications. The projects are written in C and involve socket programming, multithreading, and shell command implementation.

## Project 1: Polling System

### poller.c
`poller.c` is a server program that handles incoming client connections, processes votes, and maintains a log of voting statistics.

#### Functionality:
1. **Argument Parsing**: Parses command-line arguments and initializes variables.
2. **Socket Creation**: Creates and binds a socket to the server address.
3. **Master Thread**: 
    - Listens for incoming connections.
    - Spawns worker threads to handle client requests.
    - Uses a producer-consumer protocol for thread synchronization with mutex and condition variables.
4. **Worker Threads**:
    - Receive and process client votes.
    - Store voter information and update statistics.
    - Log activities and close connections.

#### Signal Handling:
- On receiving a `Ctrl+C` signal, the program writes the collected statistics to `pollStats` and terminates.

### pollSwayer.c
`pollSwayer.c` is a client program that reads voter information from an input file and sends it to the server.

#### Functionality:
1. **Input Reading**: Reads each line from the `InputFile`, storing names and votes in a `voters` struct.
2. **Thread Creation**: 
    - Creates a thread for each voter.
    - Each thread establishes a connection to the server, sends the voter's information, and terminates.
3. **Synchronization**: Uses mutexes to ensure thread-safe operations.

### create_input.sh
A shell script to generate a random list of voters.

#### Functionality:
1. **Argument Checking**: Validates the number of arguments and the existence of `politicalParties` file.
2. **Random Data Generation**: 
    - Reads party names from `politicalParties`.
    - Generates random names and associates them with random parties.
    - Writes the generated data to `inputFile`.

### tallyVotes.sh and processLogFile.sh
Shell scripts to process and tally votes.

#### Functionality:
1. **File Checking**: Ensures `inputFile` exists and has appropriate permissions.
2. **Vote Tallying**: 
    - Reads voter data from `inputFile`.
    - Checks for duplicate names.
    - Tallies votes for each party.
3. **Result Output**: Prints the voting results to the specified file.

### Makefile
A Makefile to compile the server and client programs.

#### Commands:
- `make`: Compiles `poller.c` and `pollSwayer.c` with pthread support.

## Project 2: Custom Shell

`mysh` is a custom shell that supports command execution, redirections, pipes, aliases, and command history.

### Files and Modules:
1. **mysh.c**: Main shell implementation.
2. **pipes.c/pipes.h**: Pipe handling.
3. **alias.c/alias.h**: Alias management.
4. **history.c/history.h**: Command history management.

### Functionality:
1. **Tokenization**: Splits the input line into tokens.
2. **Redirection Handling**: 
    - Flags for input, output, and append redirections.
    - Stores file names for redirections.
    - Manages arguments excluding redirection symbols.
3. **Pipe Handling**: 
    - Detects and handles pipes.
    - Uses separate argument arrays for commands on each side of the pipe.
    - Forks processes to execute piped commands using `execvp`.
4. **Alias Management**:
    - Checks and replaces commands with aliases.
    - Supports creating and deleting aliases.
5. **Command History**:
    - Stores commands with their IDs.
    - Provides functions to print, find, and add history entries.

### Main Loop:
- Continuously reads user input until the `exit` command is given.
- Handles redirections and pipes.
- Forks processes to execute commands, using `execvp` in child processes and waiting for completion in the parent process.
- Prints success or failure messages based on the command execution result.

### Compilation:
The project uses a Makefile to compile the various modules and link them into the `mysh` executable.

#### Commands:
- `make`: Compiles the `mysh` shell with its dependencies.

## License

This project is licensed under the MIT License. 

## Acknowledgements

This project was developed as part of the System Programming course in the Department of Informatics and Telecommunications. Special thanks to the course instructors and teaching assistants for their guidance.



