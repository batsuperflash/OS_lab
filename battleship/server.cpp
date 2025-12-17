#include <zmq.hpp>
#include <iostream>
#include <unordered_map>
#include <set>
#include <fstream>
#include <sstream>
#include <vector>

struct Player {
    std::set<std::pair<int,int>> ships;
    int wins = 0;
    int losses = 0;
};

std::unordered_map<std::string, Player> players;
std::vector<std::string> player_order;
std::string current_turn;

void load_stats() {
    std::ifstream in("stats.txt");
    std::string name;
    int w, l;
    while (in >> name >> w >> l) {
        players[name].wins = w;
        players[name].losses = l;
        players[name].ships = {{1,1},{1,2},{3,3}};
        player_order.push_back(name);
    }
    if (!player_order.empty())
        current_turn = player_order[0];
}

void save_stats() {
    std::ofstream out("stats.txt");
    for (auto &[name, p] : players) {
        out << name << " " << p.wins << " " << p.losses << "\n";
    }
}

int main() {
    load_stats();

    zmq::context_t ctx(1);
    zmq::socket_t socket(ctx, ZMQ_REP);
    socket.bind("tcp://*:5555");

    std::cout << "Battleship server starting...\nListening on tcp://*:5555\n";

    while (true) {
        zmq::message_t request;
        socket.recv(request);
        std::string msg((char*)request.data(), request.size());
        std::istringstream iss(msg);

        std::string cmd, login;
        iss >> cmd >> login;

        std::string reply;

        if (cmd == "LOGIN") {
            if (players.find(login) == players.end()) {
                players[login].ships = {{1,1},{1,2},{3,3}};
                player_order.push_back(login);
            }
            if (player_order.size() == 1)
                current_turn = login;
            reply = "OK";
        }
        else if (cmd == "SHOT") {
            int x, y;
            iss >> x >> y;

            if (players.find(login) == players.end()) {
                reply = "ERROR: PLAYER NOT LOGGED IN";
            }
            else if (player_order.size() < 2) {
                reply = "WAITING FOR SECOND PLAYER";
            }
            else if (login != current_turn) {
                reply = "ERROR: NOT YOUR TURN";
            }
            else {
                std::string other_player = (player_order[0] == login) ? player_order[1] : player_order[0];

                if (players[other_player].ships.count({x,y})) {
                    players[other_player].ships.erase({x,y});
                    reply = players[other_player].ships.empty() ? "WIN" : "HIT";

                    if (reply == "WIN") {
                        players[login].wins++;
                        players[other_player].losses++;
                        save_stats();
                        for (auto &p : player_order)
                            players[p].ships = {{1,1},{1,2},{3,3}};
                    } else {
                        current_turn = other_player;
                    }
                } else {
                    reply = "MISS";
                    current_turn = other_player;
                }
            }
        }
        else if (cmd == "STATS") {
            if (players.find(login) == players.end()) {
                reply = "ERROR: PLAYER NOT LOGGED IN";
            } else {
                auto &p = players[login];
                reply = "WINS=" + std::to_string(p.wins) +
                        " LOSSES=" + std::to_string(p.losses);
            }
        }
        else {
            reply = "ERROR: UNKNOWN COMMAND";
        }

        socket.send(zmq::buffer(reply), zmq::send_flags::none);
    }
}
