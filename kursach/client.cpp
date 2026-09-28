#include <zmq.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <unistd.h>
#include "common.hpp"

using namespace std;
using namespace chrono_literals;

int main() {
    try {
        zmq::context_t context(1);
        zmq::socket_t socket(context, ZMQ_DEALER);
        
        string server_addr = "tcp://server:5555";
        cout << "🔧 DEBUG: Connecting to server: " << server_addr << endl;
        socket.connect(server_addr);
        cout << "✅ Connected to server" << endl;
        
        string login;
        cout << "👤 Enter your login: ";
        getline(cin, login);
        
        string login_msg = "LOGIN;" + login;
        cout << "🔧 DEBUG: Sending message: '" << login_msg << "'" << endl;
        
        socket.send(zmq::buffer(login_msg), zmq::send_flags::none);
        
        cout << "⏳ Waiting for server response..." << endl;
        zmq::pollitem_t items[] = { {socket, 0, ZMQ_POLLIN, 0} };
        
        bool logged_in = false;
        string current_login = "";
        
        for (int i = 0; i < 20; i++) {
            zmq::poll(items, 1, 500ms);
            
            if (items[0].revents & ZMQ_POLLIN) {
                zmq::message_t reply;
                auto result = socket.recv(reply);
                
                if (result.has_value()) {
                    string reply_str(static_cast<char*>(reply.data()), reply.size());
                    cout << "🔧 DEBUG: Raw response length: " << reply.size() << endl;
                    cout << "📥 Received raw response: '" << reply_str << "'" << endl;
                    
                    vector<string> reply_parts = split(reply_str, ';');
                    
                    if (!reply_parts.empty()) {
                        if (reply_parts[0] == "LOGIN_SUCCESS" && reply_parts.size() >= 2) {
                            current_login = reply_parts[1];
                            cout << "🎉 Successfully logged in as " << current_login << endl;
                            logged_in = true;
                            break;
                        } else if (reply_parts[0] == "ERROR") {
                            cerr << "❌ Login failed: " << (reply_parts.size() > 1 ? reply_parts[1] : "Unknown error") << endl;
                            return 1;
                        } else {
                            cerr << "❌ Unknown response format" << endl;
                            return 1;
                        }
                    }
                }
            }
        }
        
        if (!logged_in) {
            cerr << "⏰ ERROR: No response from server after 10 seconds" << endl;
            return 1;
        }
        
        // ОСНОВНОЙ ИНТЕРАКТИВНЫЙ ЦИКЛ (ИСПРАВЛЕНО!)
        while (true) {
            cout << "\n=== 🎮 MAIN MENU ===\n";
            cout << "1. Create new game\n";
            cout << "2. Join existing game\n";
            cout << "3. View statistics\n";
            cout << "4. Exit\n";
            cout << "Choice: ";
            
            string choice_str;
            getline(cin, choice_str);  // Ожидаем ввод пользователя
            
            if (choice_str.empty()) {
                cout << "❌ Please enter a choice number" << endl;
                continue;
            }
            
            int choice;
            try {
                choice = stoi(choice_str);
            } catch (...) {
                cout << "❌ Invalid input! Please enter a number." << endl;
                continue;
            }
            
            if (choice == 1) {
                cout << "🎮 Enter game name: ";
                string game_id;
                getline(cin, game_id);
                
                string msg = "CREATE_GAME;" + game_id;
                cout << "🔧 DEBUG: Sending: '" << msg << "'" << endl;
                socket.send(zmq::buffer(msg), zmq::send_flags::none);
                
                cout << "⏳ Waiting for server response..." << endl;
                bool got_response = false;
                for (int j = 0; j < 10; j++) {
                    zmq::poll(items, 1, 500ms);
                    if (items[0].revents & ZMQ_POLLIN) {
                        zmq::message_t reply;
                        socket.recv(reply);
                        string reply_str(static_cast<char*>(reply.data()), reply.size());
                        cout << "📥 Server response: '" << reply_str << "'" << endl;
                        
                        vector<string> parts = split(reply_str, ';');
                        if (!parts.empty()) {
                            if (parts[0] == "GAME_CREATED") {
                                cout << "✅ Game '" << game_id << "' created successfully!" << endl;
                            } else if (parts[0] == "ERROR") {
                                cout << "❌ Error: " << (parts.size() > 1 ? parts[1] : "Unknown error") << endl;
                            }
                        }
                        got_response = true;
                        break;
                    }
                }
                if (!got_response) {
                    cout << "⏰ No response from server" << endl;
                }
            } 
            else if (choice == 2) {
                cout << "🔍 Enter game name to join: ";
                string game_id;
                getline(cin, game_id);
                
                string msg = "JOIN_GAME;" + game_id;
                cout << "🔧 DEBUG: Sending: '" << msg << "'" << endl;
                socket.send(zmq::buffer(msg), zmq::send_flags::none);
                
                cout << "⏳ Waiting for server response..." << endl;
                bool got_response = false;
                for (int j = 0; j < 20; j++) {
                    zmq::poll(items, 1, 500ms);
                    if (items[0].revents & ZMQ_POLLIN) {
                        zmq::message_t reply;
                        socket.recv(reply);
                        string reply_str(static_cast<char*>(reply.data()), reply.size());
                        cout << "📥 Server response: '" << reply_str << "'" << endl;
                        
                        vector<string> parts = split(reply_str, ';');
                        if (!parts.empty()) {
                            if (parts[0] == "GAME_STARTED" && parts.size() >= 4) {
                                cout << "\n\n🎉🎉🎉 GAME STARTED 🎉🎉🎉\n";
                                cout << "🆚 Opponent: " << parts[3] << endl;
                                cout << "🎯 Your turn! Enter coordinates (x y) to shoot: ";
                                
                                string coords;
                                getline(cin, coords);
                                vector<string> coord_parts = split(coords, ' ');
                                if (coord_parts.size() >= 2) {
                                    string shot_msg = "SHOT;" + coord_parts[0] + ";" + coord_parts[1];
                                    cout << "🔧 DEBUG: Sending shot: '" << shot_msg << "'" << endl;
                                    socket.send(zmq::buffer(shot_msg), zmq::send_flags::none);
                                    cout << "🔫 Shot sent! Waiting for result..." << endl;
                                    
                                    // Ждем результат выстрела
                                    for (int k = 0; k < 10; k++) {
                                        zmq::poll(items, 1, 500ms);
                                        if (items[0].revents & ZMQ_POLLIN) {
                                            zmq::message_t reply;
                                            socket.recv(reply);
                                            string reply_str(static_cast<char*>(reply.data()), reply.size());
                                            cout << "📥 Shot result: '" << reply_str << "'" << endl;
                                            
                                            vector<string> shot_parts = split(reply_str, ';');
                                            if (shot_parts.size() >= 4 && shot_parts[0] == "SHOT_RESULT") {
                                                cout << "🎯 Result: " << shot_parts[3] << " at (" << shot_parts[1] << "," << shot_parts[2] << ")" << endl;
                                                
                                                if (shot_parts.size() >= 6 && shot_parts[4] == "GAME_OVER") {
                                                    cout << "\n🎉🎉🎉 GAME OVER 🎉🎉🎉\n";
                                                    cout << "🏆 Winner: " << shot_parts[5] << endl;
                                                }
                                            }
                                            break;
                                        }
                                    }
                                }
                            } else if (parts[0] == "ERROR") {
                                cout << "❌ Error: " << (parts.size() > 1 ? parts[1] : "Unknown error") << endl;
                            }
                        }
                        got_response = true;
                        break;
                    }
                }
                if (!got_response) {
                    cout << "⏰ Waiting for server response..." << endl;
                }
            } 
            else if (choice == 3) {
                cout << "📊 Requesting statistics..." << endl;
                socket.send(zmq::buffer("STATS"), zmq::send_flags::none);
                
                bool got_response = false;
                for (int j = 0; j < 10; j++) {
                    zmq::poll(items, 1, 200ms);
                    if (items[0].revents & ZMQ_POLLIN) {
                        zmq::message_t reply;
                        socket.recv(reply);
                        string reply_str(static_cast<char*>(reply.data()), reply.size());
                        cout << "📥 Server response: '" << reply_str << "'" << endl;
                        
                        vector<string> stats = split(reply_str, ';');
                        if (stats.size() >= 3 && stats[0] == "STATS") {
                            cout << "\n=== 📈 YOUR STATISTICS ===\n";
                            cout << "🏆 Wins: " << stats[1] << endl;
                            cout << "❌ Losses: " << stats[2] << endl;
                        }
                        got_response = true;
                        break;
                    }
                }
                if (!got_response) {
                    cout << "⏰ Server is busy, try again later." << endl;
                }
            } 
            else if (choice == 4) {
                cout << "👋 Exiting game..." << endl;
                break;
            }
            else {
                cout << "❌ Invalid choice! Try again.\n";
            }
        }
        
    } catch (const exception& e) {
        cerr << "💥 CRITICAL ERROR: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}