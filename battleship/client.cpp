#include <zmq.hpp>
#include <iostream>
#include <string>

int main() {
    zmq::context_t ctx(1);
    zmq::socket_t socket(ctx, ZMQ_REQ);
    socket.connect("tcp://localhost:5555");

    std::string login;
    std::cout << "Login: ";
    std::cin >> login;

    socket.send(zmq::buffer("LOGIN " + login), zmq::send_flags::none);
    zmq::message_t reply;
    socket.recv(reply);

    std::cout << "Server: " << std::string((char*)reply.data(), reply.size()) << "\n";

    while (true) {
        std::cout << "\n1) Shot\n2) Stats\n3) Exit\n> ";
        int cmd;
        std::cin >> cmd;

        if (cmd == 1) {
            int x, y;
            std::cout << "Enter x y: ";
            std::cin >> x >> y;
            socket.send(
                zmq::buffer("SHOT " + login + " " + std::to_string(x) + " " + std::to_string(y)),
                zmq::send_flags::none
            );
        }
        else if (cmd == 2) {
            socket.send(zmq::buffer("STATS " + login), zmq::send_flags::none);
        }
        else {
            break;
        }

        socket.recv(reply);
        std::cout << "Server: "
                  << std::string((char*)reply.data(), reply.size())
                  << "\n";
    }
}
