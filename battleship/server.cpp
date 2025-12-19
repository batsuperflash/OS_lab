#include <zmq.hpp>
#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>
#include <sstream>
#include <fstream> //в strace: openat, read, mmap — загрузка libzmq, libstdc++, libc

struct Player {
    std::set<std::pair<int,int>> ships{{1,1},{1,2},{3,3}};
    int wins = 0;
    int losses = 0;
}; //в strace: openat, read, mmap — загрузка libzmq, libstdc++, libc

struct Game {
    std::vector<std::string> players;
    std::string current_turn;
};

std::unordered_map<std::string, Player> players;
std::unordered_map<std::string, Game> games;

void save_stats() { //в strace: openat("stats.txt", O_WRONLY|O_CREAT|O_TRUNC)
    std::ofstream out("stats.txt");
    for (auto &[name, p] : players)
        out << name << " " << p.wins << " " << p.losses << "\n"; //в strace: write, close
} 

int main() { //exeve
    zmq::context_t ctx(1); //
    zmq::socket_t socket(ctx, ZMQ_REP); //в strace: execve("./server", ...)
    socket.bind("tcp://*:5555"); //strace: socket(...), bind, listen создание сокета и привязка к порту и режим ожидания от клиента

    std::cout << "Battleship server started\n"; //вывод in strace: write

    while (true) {
        zmq::message_t req; //ожидаем сообщ от клиента
        socket.recv(req);  //recvform
        std::string msg((char*)req.data(), req.size());
        std::istringstream iss(msg);

        std::string cmd;
        iss >> cmd;

        std::string reply = "ERROR";

        if (cmd == "CREATE_GAME") {
            std::string game, login;
            iss >> game >> login;

            if (!games.count(game)) {
                games[game].players.push_back(login);
                games[game].current_turn = login;
                players[login]; // создаём игрока
                reply = "GAME_CREATED";
            } else {
                reply = "GAME_EXISTS";
            }
        }

        else if (cmd == "JOIN_GAME") {
            std::string game, login;
            iss >> game >> login;

            if (games.count(game) && games[game].players.size() == 1) {
                games[game].players.push_back(login);
                players[login];
                reply = "JOINED";
            } else {
                reply = "CANNOT_JOIN";
            }
        }

        else if (cmd == "SHOT") {
            std::string game, login;
            int x, y;
            iss >> game >> login >> x >> y;

            if (!games.count(game)) {
                reply = "NO_SUCH_GAME";
            } else {
                auto &g = games[game];

                if (login != g.current_turn) {
                    reply = "NOT_YOUR_TURN";
                } else {
                    std::string enemy =
                        (g.players[0] == login) ? g.players[1] : g.players[0];

                    if (players[enemy].ships.count({x,y})) {
                        players[enemy].ships.erase({x,y});
                        if (players[enemy].ships.empty()) {
                            players[login].wins++;
                            players[enemy].losses++;
                            save_stats();
                            reply = "WIN";
                        } else {
                            reply = "HIT";
                        }
                    } else {
                        reply = "MISS";
                    }
                    g.current_turn = enemy;
                }
            }
        }

        else if (cmd == "STATS") {
            std::string login;
            iss >> login;
            reply = "WINS=" + std::to_string(players[login].wins) +
                    " LOSSES=" + std::to_string(players[login].losses);
        }

        socket.send(zmq::buffer(reply), zmq::send_flags::none); //sendto
    }
}
