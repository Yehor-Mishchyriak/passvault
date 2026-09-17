#include "terminal.hpp"

#include <iostream>

namespace {

std::string PROMPT_START = "passvault> ";
std::string INPUT_START = "your_command> ";

} // namespace

void send_message(const std::string& msg){
    std::cout << PROMPT_START << msg << "\n";
}

void request_input(){
    std::cout << INPUT_START;
}

bool read_raw_input(std::string& usr_command){
    bool success{std::getline(std::cin, usr_command)};
    return success;
}
