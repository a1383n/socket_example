# TCP Socket Example with Multi-Threading using Epoll

This project provides a simple TCP socket server example implemented in C, demonstrating multi-threading using epoll for efficient event handling. It serves as a sample project to understand basic socket programming concepts.

### Using Make

1. Clone this repository.
2. Navigate to the project directory.
3. Run `make` to build the server executable using GCC directly.
4. Run executable: `./server`

### Using CMake

1. Ensure you have CMake installed on your system.
2. Clone this repository.
3. Navigate to the project directory.
4. Create a build directory: `mkdir build && cd build`.
5. Run CMake: `cmake ..`.
6. Build the project: `cmake --build .`.
7. Run executable: `./socket_example`

## Usage

1. After building the project, run the executable generated.
2. The server will listen for incoming connections on port 3000.
3. Connect to the server using a TCP client (e.g., telnet, netcat).
4. Send messages to the server, and it will echo them back.

## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, feel free to open an issue or submit a pull request.

## License

This project is licensed under the GPL-3 License. See the [LICENSE](LICENSE) file for details.
