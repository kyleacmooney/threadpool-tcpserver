#include "tcp_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace netserver {

TcpServer::TcpServer(int port, int backlog)
    : port_(port)
    , backlog_(backlog)
    , server_fd_(-1)
    , running_(false)
{
}

TcpServer::~TcpServer() {
    stop();
}

void TcpServer::start() {
    // Create socket
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }

    // Set socket options - allow address reuse
    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to set socket options: " + std::string(strerror(errno)));
    }

    // Bind to address and port
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections on any interface
    address.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to bind to port " + std::to_string(port_) +
                                 ": " + std::string(strerror(errno)));
    }

    // Start listening
    if (listen(server_fd_, backlog_) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to listen on socket: " + std::string(strerror(errno)));
    }

    running_.store(true);
    std::cout << "Server listening on port " << port_ << std::endl;
}

int TcpServer::acceptConnection() {
    if (!running_.load()) {
        return -1;
    }

    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    int client_fd = accept(server_fd_, (struct sockaddr*)&client_address, &client_len);

    if (client_fd < 0) {
        if (errno != EINTR && running_.load()) {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
        }
        return -1;
    }

    return client_fd;
}

void TcpServer::stop() {
    running_.store(false);

    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
        std::cout << "Server stopped" << std::endl;
    }
}

} // namespace netserver
