#include <zmq.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "common.hpp"

using namespace std;

// Глобальные структуры данных
unordered_map<string, string> identity_to_login;   // identity -> login
unordered_map<string, int> player_wins;             // login -> wins
unordered_map<string, int> player_losses;           // login -> losses
unordered_map<string, string> player_current_game; // login -> game_id

// Структура игры
struct Game {
    string id;
    string player1;
    string player2;
    vector<vector<int>> board1; // 0=пусто, 1=корабль, 2=попадание, 3=промах
    vector<vector<int>> board2;
    string current_turn;        // чей ход
    bool active = false;        // true когда оба игрока подключены
};
unordered_map<string, Game> games;

void send_message(zmq::socket_t& socket, const string& identity, const string& message) {
    // Отправляем identity
    zmq::message_t reply_identity(identity.data(), identity.size());
    socket.send(reply_identity, zmq::send_flags::sndmore);
    
    // Отправляем сообщение (без пустого delimiter!)
    zmq::message_t reply(message.data(), message.size());
    socket.send(reply, zmq::send_flags::none);
    
    cout << "📨 Sent to [" << identity << "]: '" << message << "'" << endl;
}

void generate_board(vector<vector<int>>& board) {
    // Генерация поля 10x10 с одним кораблём для простоты
    board = vector<vector<int>>(10, vector<int>(10, 0));
    board[3][4] = 1; // Корабль в позиции (3,4)
}

int main() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_ROUTER);
    
    string bind_addr = "tcp://0.0.0.0:5555";
    socket.bind(bind_addr);
    
    cout << "⚓ SEA BATTLE SERVER STARTED" << endl;
    cout << "🌐 Listening on " << bind_addr << endl;
    cout << "⏱️  PID: " << getpid() << endl;
    cout << "🐳 Running in Docker container" << endl;
    cout << "===================================" << endl;
    cout << "🔧 DEBUG: Using modern ROUTER-DEALER protocol (no empty delimiter)" << endl;

    while (true) {
        zmq::message_t identity;
        zmq::message_t message;

        try {
            // Получаем identity и message (без delimiter!)
            socket.recv(identity);
            socket.recv(message);

            string identity_str(static_cast<char*>(identity.data()), identity.size());
            string msg_str(static_cast<char*>(message.data()), message.size());
            
            cout << "🔧 DEBUG: Raw identity length: " << identity.size() << endl;
            cout << "🔧 DEBUG: Raw message length: " << message.size() << endl;
            cout << "📥 Received from [" << identity_str << "]: '" << msg_str << "'" << endl;
            
            vector<string> parts = split(msg_str, ';');
            if (parts.empty()) {
                cerr << "❌ ERROR: Empty message received" << endl;
                continue;
            }
            
            string cmd = parts[0];
            cout << "🔧 DEBUG: Parsed command: '" << cmd << "'" << endl;

            // Аутентификация нового клиента
            if (identity_to_login.find(identity_str) == identity_to_login.end()) {
                if (cmd == "LOGIN" && parts.size() >= 2) {
                    string login = parts[1];
                    if (player_wins.find(login) != player_wins.end()) {
                        send_message(socket, identity_str, "ERROR;Login already exists");
                    } else {
                        identity_to_login[identity_str] = login;
                        player_wins[login] = 0;
                        player_losses[login] = 0;
                        player_current_game[login] = "";
                        send_message(socket, identity_str, "LOGIN_SUCCESS;" + login);
                        cout << "✅ New player: " << login << endl;
                    }
                } else {
                    send_message(socket, identity_str, "ERROR;Login required: LOGIN;<username>");
                }
                continue;
            }

            string login = identity_to_login[identity_str];
            cout << "🔧 DEBUG: Authenticated player: " << login << endl;

            // Обработка команд
            if (cmd == "CREATE_GAME" && parts.size() >= 2) {
                string game_id = parts[1];
                cout << "🔧 DEBUG: Creating game: " << game_id << endl;
                
                // Проверки
                if (games.find(game_id) != games.end()) {
                    cout << "❌ ERROR: Game already exists" << endl;
                    send_message(socket, identity_str, "ERROR;Game exists");
                } else if (!player_current_game[login].empty()) {
                    cout << "❌ ERROR: Player already in game" << endl;
                    send_message(socket, identity_str, "ERROR;Already in game");
                } else {
                    // Создаем новую игру
                    Game new_game;
                    new_game.id = game_id;
                    new_game.player1 = login;
                    generate_board(new_game.board1);
                    games[game_id] = new_game;
                    player_current_game[login] = game_id;
                    
                    cout << "✅ Game created: " << game_id << " by " << login << endl;
                    send_message(socket, identity_str, "GAME_CREATED;" + game_id);
                }
            } 
            else if (cmd == "JOIN_GAME" && parts.size() >= 2) {
                string game_id = parts[1];
                cout << "🔧 DEBUG: Joining game: " << game_id << endl;
                
                if (games.find(game_id) == games.end()) {
                    cout << "❌ ERROR: Game not found" << endl;
                    send_message(socket, identity_str, "ERROR;Game not found");
                } else if (!player_current_game[login].empty()) {
                    cout << "❌ ERROR: Player already in game" << endl;
                    send_message(socket, identity_str, "ERROR;Already in game");
                } else {
                    Game& game = games[game_id];
                    if (!game.player2.empty()) {
                        cout << "❌ ERROR: Game full" << endl;
                        send_message(socket, identity_str, "ERROR;Game full");
                    } else {
                        game.player2 = login;
                        generate_board(game.board2);
                        game.active = true;
                        game.current_turn = game.player1; // Первый игрок начинает
                        player_current_game[login] = game_id;
                        
                        // Уведомить обоих игроков
                        string start_msg = "GAME_STARTED;" + game_id + ";" + game.player1 + ";" + game.player2;
                        for (const auto& id_map : identity_to_login) {
                            if (id_map.second == game.player1 || id_map.second == game.player2) {
                                send_message(socket, id_map.first, start_msg);
                            }
                        }
                        cout << "✅ " << login << " joined game: " << game_id << endl;
                    }
                }
            } 
            else if (cmd == "SHOT" && parts.size() >= 3) {
                if (player_current_game[login].empty()) {
                    cout << "❌ ERROR: Not in game" << endl;
                    send_message(socket, identity_str, "ERROR;Not in game");
                    continue;
                }
                
                string game_id = player_current_game[login];
                if (games.find(game_id) == games.end()) {
                    cout << "❌ ERROR: Game not found" << endl;
                    send_message(socket, identity_str, "ERROR;Game not found");
                    continue;
                }
                
                Game& game = games[game_id];
                
                if (game.current_turn != login) {
                    cout << "❌ ERROR: Not your turn" << endl;
                    send_message(socket, identity_str, "ERROR;Not your turn");
                    continue;
                }
                
                int x, y;
                try {
                    x = stoi(parts[1]);
                    y = stoi(parts[2]);
                } catch (...) {
                    cout << "❌ ERROR: Invalid coordinates format" << endl;
                    send_message(socket, identity_str, "ERROR;Invalid coordinates format");
                    continue;
                }
                
                // Проверка границ координат
                if (x < 0 || x >= 10 || y < 0 || y >= 10) {
                    cout << "❌ ERROR: Invalid coordinates" << endl;
                    send_message(socket, identity_str, "ERROR;Invalid coordinates");
                    continue;
                }
                
                // Определение чьё поле атаковать
                vector<vector<int>>& target_board = 
                    (login == game.player1) ? game.board2 : game.board1;
                
                string result;
                if (target_board[x][y] == 1) {
                    target_board[x][y] = 2; // Попадание
                    result = "HIT";
                } else {
                    target_board[x][y] = 3; // Промах
                    result = "MISS";
                }
                
                // Проверка победы (упрощённо)
                bool game_over = false;
                string winner;
                if (result == "HIT" && target_board[3][4] == 2) { // Условие победы
                    game_over = true;
                    winner = login;
                    player_wins[winner]++;
                    string loser = (winner == game.player1) ? game.player2 : game.player1;
                    player_losses[loser]++;
                }
                
                // Смена хода
                game.current_turn = (login == game.player1) ? game.player2 : game.player1;
                
                // Уведомление игроков
                string msg = "SHOT_RESULT;" + to_string(x) + ";" + to_string(y) + ";" + result;
                if (game_over) {
                    msg += ";GAME_OVER;" + winner;
                    games.erase(game_id);
                    player_current_game[game.player1] = "";
                    player_current_game[game.player2] = "";
                }
                
                for (const auto& id_map : identity_to_login) {
                    if (id_map.second == game.player1 || id_map.second == game.player2) {
                        send_message(socket, id_map.first, msg);
                    }
                }
                cout << "🎯 Shot processed: " << x << "," << y << " = " << result << endl;
            } 
            else if (cmd == "STATS") {
                string msg = "STATS;" + 
                            to_string(player_wins[login]) + ";" + 
                            to_string(player_losses[login]);
                send_message(socket, identity_str, msg);
                cout << "📊 Stats sent for: " << login << endl;
            } 
            else {
                cout << "❌ ERROR: Unknown command: " << cmd << endl;
                send_message(socket, identity_str, "ERROR;Unknown command: " + cmd);
            }
            
        } catch (const zmq::error_t& e) {
            cerr << "❌ ZeroMQ Error: " << e.what() << endl;
        } catch (const exception& e) {
            cerr << "❌ General Error: " << e.what() << endl;
        }
    }
    
    return 0;
}