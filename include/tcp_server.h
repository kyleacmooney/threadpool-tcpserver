#pragma once

#include <string>
#include <memory>
#include <atomic>

namespace netserver {

/**
 * TCP Server - Handles accepting connections and basic socket operations
 *
 * This class manages:
 * - Socket creation and binding
 * - Listening for incoming connections
 * - Accepting new client connections
 */
class TcpServer {
public:
    /**
     * Construct a TCP server
     * @param port Port number to bind to
     * @param backlog Maximum length of pending connection queue
     */
    TcpServer(int port, int backlog = 128);

    /**
     * Destructor - ensures proper socket cleanup
     */
    ~TcpServer();

    // Prevent copying
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    /**
     * Start the server - binds and listens on the configured port
     * @throws std::runtime_error if binding or listening fails
     */
    void start();

    /**
     * Accept a new client connection (blocking)
     * @return File descriptor for the new connection, or -1 on error
     */
    int acceptConnection();

    /**
     * Stop the server and close the listening socket
     */
    void stop();

    /**
     * Check if server is running
     */
    bool isRunning() const { return running_.load(); }

    /**
     * Get the port the server is bound to
     */
    int getPort() const { return port_; }

private:
    int port_;              // Port to bind to
    int backlog_;           // Connection backlog size
    int server_fd_;         // Server socket file descriptor
    std::atomic<bool> running_; // Server running state
};

} // namespace netserver
