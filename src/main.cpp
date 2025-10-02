#include "tcp_server.h"
#include <iostream>
#include <csignal>
#include <atomic>
#include <unistd.h>

// Global flag for graceful shutdown
std::atomic<bool> shutdown_requested(false);

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nShutdown signal received..." << std::endl;
        shutdown_requested.store(true);
    }
}

int main(int argc, char* argv[]) {
    // Default port
    int port = 8080;

    // Simple command-line argument parsing
    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid port number: " << argv[1] << std::endl;
            return 1;
        }
    }

    // Set up signal handlers for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        // Create and start server
        netserver::TcpServer server(port);
        server.start();

        std::cout << "Server started successfully. Press Ctrl+C to stop." << std::endl;

        // Simple accept loop (we'll improve this with thread pool soon)
        while (!shutdown_requested.load() && server.isRunning()) {
            int client_fd = server.acceptConnection();

            if (client_fd >= 0) {
                std::cout << "New connection accepted (fd: " << client_fd << ")" << std::endl;

                // For now, just send a simple message and close
                const char* message = "Hello from server!\n";
                write(client_fd, message, strlen(message));
                close(client_fd);

                std::cout << "Connection closed" << std::endl;
            }
        }

        server.stop();
        std::cout << "Server shutdown complete" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
