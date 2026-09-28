#include <zmq.hpp>
#include <iostream>
#include <string>
#include <unistd.h>

int main() {
    try {
        std::cout << "=== CONNECTION TEST ===" << std::endl;
        
        // Проверяем доступность сервера
        zmq::context_t context(1);
        zmq::socket_t socket(context, ZMQ_REQ);
        socket.set(zmq::sockopt::sndtimeo, 3000);  // 3 секунды таймаут отправки
        socket.set(zmq::sockopt::rcvtimeo, 3000);  // 3 секунды таймаут получения
        
        std::cout << "[1/4] Connecting to tcp://localhost:5555..." << std::endl;
        socket.connect("tcp://localhost:5555");
        
        std::cout << "[2/4] Sending test message..." << std::endl;
        zmq::message_t request(4);
        memcpy(request.data(), "PING", 4);
        socket.send(request, zmq::send_flags::none);
        
        std::cout << "[3/4] Waiting for response..." << std::endl;
        zmq::message_t reply;
        bool received = socket.recv(reply, zmq::recv_flags::dontwait);
        
        if (received) {
            std::string reply_str(static_cast<char*>(reply.data()), reply.size());
            std::cout << "[4/4] SUCCESS! Received reply: " << reply_str << std::endl;
            return 0;
        } else {
            std::cout << "[4/4] FAILED! No response from server." << std::endl;
            std::cout << "[DEBUG] Possible issues:" << std::endl;
            std::cout << "  - Server not running" << std::endl;
            std::cout << "  - Server not listening on port 5555" << std::endl;
            std::cout << "  - Firewall blocking connection" << std::endl;
            std::cout << "  - Protocol mismatch (ROUTER vs REQ)" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
        return 1;
    }
}