#include <zmq.hpp>
#include <iostream>
#include <string>

int main() {
    zmq::context_t ctx(1); //
    zmq::socket_t socket(ctx, ZMQ_REQ); //socket() //оздание сокета с паттерн запрос-ответ
    socket.connect("tcp://localhost:5555"); //connect() //одключение/привязка сокета, скрывает: socket(), connect() настройки TCP

    std::string game, login;

    std::cout << "1) Create game\n2) Join game\n> ";
    int c;
    std::cin >> c; //read()

    std::cout << "Game name: ";
    std::cin >> game; //read()

    std::cout << "Login: ";
    std::cin >> login; //read()

    std::string cmd = (c == 1 ? "CREATE_GAME " : "JOIN_GAME ");
    socket.send(zmq::buffer(cmd + game + " " + login), zmq::send_flags::none); //send() //Скрывает: буферизацию, сериализацию

    zmq::message_t reply;
    socket.recv(reply); //Скрывает: десериализацию
    std::cout << "Server: " << reply.to_string() << "\n"; //write()

    while (true) {
        std::cout << "\n1) Shot\n2) Stats\n3) Exit\n> "; 
        int x;
        std::cin >> x; //read() 

        if (x == 1) {
            int a,b;
            std::cout << "x y: ";
            std::cin >> a >> b;
            socket.send( //send()
                zmq::buffer("SHOT " + game + " " + login + " " +
                            std::to_string(a) + " " + std::to_string(b)),
                zmq::send_flags::none
            );
        }
        else if (x == 2) { //send()
            socket.send(zmq::buffer("STATS " + login), zmq::send_flags::none);
        }
        else break;

        socket.recv(reply); 
        std::cout << "Server: " << reply.to_string() << "\n" ; //write()
    }
}
