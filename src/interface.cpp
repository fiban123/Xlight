#include "../include/interface.hpp"

void TerminalInterface::init(VisualizationAlgorithm* algptr) {
    algo = algptr;
}

void TerminalInterface::start() {
    interface_thread = thread(interface_loop, this);
    running = true;
    cout << "terminal interface started" << endl;
}

void TerminalInterface::stop() {
    running = false;
    interface_thread.join();
    cout << "terminal interface stopped" << endl;
}

vector<string> TerminalInterface::parse_cmd(string cmd) {
    vector<string> parsed;

    size_t c = 0;
    for (size_t i = 0; i < cmd.size(); i++){
        if (cmd[i] == ' '){
            parsed.push_back(cmd.substr(c, i - c));
            c = i + 1;
        }
    }
    parsed.push_back(cmd.substr(c, cmd.size()));

    return parsed;
}

void TerminalInterface::interface_loop() {
    while (running){
        string cmd;
        getline(cin, cmd);
        
        vector<string> parsed = parse_cmd(cmd);
        if (parsed.size() > 2){
            if (parsed[0] == "setalgo"){
                float new_value = stof(parsed[2]);
                algo->change_setting(parsed[1], new_value);
            }
        }
    }
}

TerminalInterface::~TerminalInterface() {
    if (running){
        running = false;
        interface_thread.detach();
        cout << "terminal interface terminated" << endl;
    }
}
