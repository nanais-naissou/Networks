# Network Mastermind — Client/Server Application

This project was developed as part of a **Computer Networks practical assignment (TP 5)**. The goal is to implement a networked version of the **Mastermind** game using a client/server architecture and TCP sockets in C.

## Table of Contents

* [Project Overview](#project-overview)
* [Objectives](#objectives)
* [Architecture](#architecture)
* [Game Rules](#game-rules)
* [Client/Server Protocol](#clientserver-protocol)
* [Technical Choices](#technical-choices)
* [Project Structure](#project-structure)
* [Compilation](#compilation)
* [Usage](#usage)
* [Example](#example)
* [Testing](#testing)
* [Possible Improvements](#possible-improvements)
* [Technologies](#technologies)

---

## Project Overview

The project consists of implementing the **Mastermind game over a network**.

A client connects to a server using a TCP connection. The server generates a secret combination of colors, while the client allows the player to submit guesses.

After each guess, the server sends back:

* The number of colors that are in the correct position.
* The number of colors that are correct but in the wrong position.

The game ends when the player finds the complete secret combination.

The game supports the following eight colors:

* Red
* Yellow
* Green
* Blue
* Orange
* White
* Purple
* Fuchsia

The player chooses the number of colors to guess at the beginning of the game.

---

## Objectives

The main objectives of this project are to practice:

* Client/server architecture
* TCP/IP communication
* Socket programming in C
* Network connection management
* TCP socket primitives
* Application-level protocol design
* Error handling
* Iterative server implementation
* Network application testing

The project makes use of standard socket functions such as:

```c
socket()
bind()
listen()
accept()
connect()
read()
write()
close()
```

---

## Architecture

The application follows a traditional client/server architecture.

```text
                   TCP Connection
        ┌─────────────────────────────────┐
        │                                 │
        ▼                                 ▼
+---------------+                 +---------------+
|    CLIENT     | <-------------> |    SERVER     |
|               |       TCP       |               |
| Player input  |                 | Secret code   |
| Guess display |                 | Game logic    |
+---------------+                 +---------------+
```

### Client

The client is responsible for:

1. Connecting to the server.
2. Choosing the game difficulty.
3. Sending guesses to the server.
4. Receiving the results of each guess.
5. Displaying the results to the player.
6. Continuing until the game is completed.

### Server

The server is responsible for:

1. Creating a TCP socket.
2. Binding the socket to an address and port.
3. Listening for incoming connections.
4. Accepting client connections.
5. Generating the secret combination.
6. Receiving player guesses.
7. Comparing guesses with the secret combination.
8. Sending the results back to the client.
9. Ending the game when the correct combination is found.

---

## Game Rules

At the beginning of the game, the client chooses the number of colors that must be guessed.

For example, with four colors, the secret combination could be:

```text
Red Blue Green Yellow
```

The player then submits guesses.

For every guess, the server calculates two values.

### Correctly Placed Colors

A color is correctly placed if it is the same color and at the same position as the corresponding color in the secret combination.

Example:

```text
Secret:      Red   Blue   Green   Yellow
Guess:       Red   Blue   White   Yellow

Correctly placed: 3
```

### Correct Colors in the Wrong Position

A color is incorrectly placed if it exists in the secret combination but is located at a different position.

The server must correctly handle duplicate colors when calculating the result.

The game ends when all colors are correctly placed.

---

## Client/Server Protocol

The application uses **TCP** for communication between the client and the server.

TCP was chosen because the game requires reliable and ordered communication. Every guess must be correctly received by the server, and every response must be correctly received by the client.

The application protocol consists of a sequence of messages exchanged between the client and the server.

```text
Client                              Server
  |                                    |
  |-------- TCP connection ----------->|
  |                                    |
  |-------- Number of colors --------->|
  |                                    |
  |<------- Confirmation --------------|
  |                                    |
  |-------- Player guess ------------->|
  |                                    |
  |<------- Game result ---------------|
  |                                    |
  |-------- Next guess --------------->|
  |                                    |
  |<------- Game result ---------------|
  |                                    |
  |                 ...                |
  |                                    |
  |<------- Victory -------------------|
  |                                    |
  |-------- Connection close ---------->|
```

The exact format of the messages is defined in the source code.

---

## Technical Choices

### TCP

TCP was selected as the transport protocol because it provides:

* Reliable data transmission
* Ordered delivery
* Connection-oriented communication
* Error detection and retransmission
* A persistent connection between the client and server

These characteristics are well suited to a turn-based game such as Mastermind.

### Iterative Server

The first version of the project uses an **iterative server**.

The server handles clients sequentially:

```text
Server
  |
  +---- Client 1
  |       |
  |       +---- Game
  |       +---- End
  |
  +---- Client 2
  |       |
  |       +---- Game
  |       +---- End
  |
  +---- Client 3
          |
          +---- Game
          +---- End
```

This approach keeps the implementation relatively simple and is sufficient for the first version of the application.

A concurrent server could be implemented later to allow multiple clients to play simultaneously.

---

## Project Structure

```text
.
├── README.md
├── client.c
├── serveur.c
├── fon.c
├── Makefile
└── Documentation/
    └── ...
```

### `client.c`

Contains the client implementation:

* Server connection
* User input
* Game interface
* Guess generation
* Sending data
* Receiving server responses

### `serveur.c`

Contains the server implementation:

* Socket creation
* Socket configuration
* Client connection handling
* Secret combination generation
* Guess processing
* Result calculation
* Response transmission

### `fon.c`

Contains the helper functions provided for the practical assignment. These functions simplify socket usage and error handling.

### `Makefile`

Provides commands for compiling the client and server.

---

## Compilation

The project can be compiled using:

```bash
make
```

This should generate the client and server executables:

```text
client
serveur
```

To remove compiled files:

```bash
make clean
```

The project requires a C compiler and an environment compatible with the socket functions used by the application.

---

## Usage

### Start the Server

On the server machine:

```bash
./serveur <port>
```

For example:

```bash
./serveur 5000
```

The server will then wait for incoming client connections.

### Start the Client

On the client machine:

```bash
./client <server_ip> <port>
```

For example:

```bash
./client 127.0.0.1 5000
```

When testing on two different machines, replace `127.0.0.1` with the IP address of the machine running the server.

---

## Example

A typical client session may look like:

```text
$ ./client 127.0.0.1 5000

Connecting to server...

Welcome to Mastermind.

Choose the number of colors:
4

Available colors:
1 - Red
2 - Yellow
3 - Green
4 - Blue
5 - Orange
6 - White
7 - Purple
8 - Fuchsia

Enter your guess:
Red Blue Green Yellow

Result:
3 correctly placed
0 incorrectly placed

Enter your next guess:
...
```

The player continues submitting guesses until the secret combination is found.

---

## Testing

Several tests can be performed to verify the application.

### Connection Test

Verify that the client can successfully establish a TCP connection with the server.

### Difficulty Test

Verify that the number of colors selected by the client is correctly transmitted to the server.

### Correct Position Test

Use a guess containing several correctly positioned colors and verify the result returned by the server.

### Incorrect Position Test

Use a guess containing colors that exist in the secret combination but are located at different positions.

### Victory Test

Send the exact secret combination and verify that the server correctly detects the end of the game.

### Multiple Client Test

Connect several clients sequentially and verify that the server correctly returns to the listening state after each game.

---

## Possible Improvements

Several improvements could be implemented in future versions.

### Concurrent Server

The server could handle several players simultaneously by creating a dedicated process or thread for each client.

```text
                    Server
                      |
          +-----------+-----------+
          |           |           |
          v           v           v
       Client 1    Client 2    Client 3
        Game        Game        Game
```

### Multiplayer Ranking

The server could keep track of the number of attempts made by each player and create a ranking based on the game results.

### Input Validation

More extensive validation could be added to ensure that:

* The selected number of colors is valid.
* All submitted colors are valid.
* The number of submitted colors matches the selected difficulty.
* Received network messages have the expected format.

### Improved User Interface

The terminal interface could be improved using the `curses` library to make the game easier and more pleasant to use.

### Statistics

The application could also provide:

* Number of attempts
* Best scores
* Game duration
* Guess history

---

## Technologies

| Technology  | Usage                       |
| ----------- | --------------------------- |
| C           | Application development     |
| TCP/IP      | Network communication       |
| BSD Sockets | Client/server communication |
| Make        | Build automation            |
| Unix/Linux  | Development environment     |

---

## Academic Context

This project was developed as part of a **Computer Networks practical assignment**.

It focuses on the implementation of a network application using the client/server model and TCP sockets.

The project demonstrates practical knowledge of:

* Client/server communication
* TCP/IP protocols
* Socket programming
* Application-level protocols
* Iterative servers
* Network error handling
* Distributed application design

---

## Authors

Project developed as part of the **Computer Networks — TP 5**.

**Language:** C
**Network protocol:** TCP/IP
**Architecture:** Client/Server
**Application:** Mastermind

