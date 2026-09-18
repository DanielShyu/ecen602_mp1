# TCP Echo Client and Server

This project implements a TCP echo service using IPv4 sockets. The server accepts connections and creates one child process per client. The client sends input from `stdin` and prints the echoed response.

## Team Responsibilities

| Member | Responsibility |
| --- | --- |
| Lian Chu | Server implementation (`server.cpp`) |
| Daniel Shyu | Client implementation (`client.cpp`) |

## Architecture

```text
stdin -> client -- TCP/IPv4 --> server child -> TCP/IPv4 --> client -> stdout
                                  ^
                                  |
                         server parent accepts
```

## Build

```sh
make          # builds client and server
make clean    # removes executables
```

The project requires a Unix-like environment such as Linux or WSL because it uses POSIX sockets and `fork()`. The compiler flags are `-Wall -Wextra -std=c++17`.

## Usage

Start the server first:

```sh
./server <port>
```

In another terminal, start the client:

```sh
./client <server-ip> <port>
```

Example:

```sh
./server 5000
./client 127.0.0.1 5000
```


## Protocol

- Transport: TCP over IPv4.
- Message format: newline-terminated input.
- Server buffer size: 1024 bytes, including the null terminator.
- The server returns the received bytes without modification.


## Files

| File | Description |
| --- | --- |
| `server.cpp` | Forking TCP echo server |
| `client.cpp` | TCP echo client |
| `makefile` | Build and cleanup commands |
