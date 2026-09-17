#include <iostream>
#include <format>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>

/* 
 CORE DATA TYPES & CONSTANTS:
*/
enum MainLoopState{
    RUNNING,
    ABORTED
};

enum Command{
    AddEntry,
    EditEntry,
    DeleteEntry,
    Quit,
    Help,
    Invalid,
    None
};

std::string NULL_STR = "NULL";

std::unordered_map<std::string, Command> commands = {
    {"/add_entry", AddEntry},
    {"/edit_entry", EditEntry},
    {"/delete_entry", DeleteEntry},
    {"/quit", Quit},
    {"/help", Help}
};

struct Entry{
    std::string name = NULL_STR;
    std::string username = NULL_STR;
    std::string secret = NULL_STR;
};

using Vault = std::unordered_map<std::string, Entry>;

std::string PROMPT_START = "passvault> ";
std::string INPUT_START = "your_command> ";
std::vector<std::string> reserved_words{NULL_STR};
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* 
 Utility functions:
*/
template<typename K, typename V>
V get_value_from(
    const std::unordered_map<K, V>& map,
    const K& key,
    const V& default_value
){
    auto it = map.find(key);
    return it != map.end() ? it->second : default_value;
}

template<typename K, typename V>
bool is_in(const K& key, const std::unordered_map<K, V>& map){
    auto it = map.find(key);
    return (it != map.end());
}

bool is_reserved(const std::string& s){
    return std::find(reserved_words.begin(), reserved_words.end(), s) != reserved_words.end();
}
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* 
 SYSTEM ACTIONS:
*/
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

void quit(MainLoopState& MLS){
    MLS = ABORTED;
}

void add_entry(std::string& raw_usr_input, Vault& secrets){
    Entry new_entry;
    /* -=-=-=-    NAME   -=-=-=- */
    send_message("Enter the NAME of the secret: ");
    request_input(); read_raw_input(raw_usr_input); auto name = raw_usr_input;
    if (is_reserved(name)){
        send_message(std::format("\"{}\" is a reserved word. Please enter a different NAME.", name));
        return;
    }
    if (is_in(name, secrets)){
        send_message(std::format("\"{}\" A secret with the provided NAME already exists. Pick a new NAME or edit the existing entry.", name));
        return;
    }
    new_entry.name = raw_usr_input;
    /* -=-=-=- USER_NAME -=-=-=- */
    send_message("Enter the associated USER_NAME: ");
    request_input(); read_raw_input(raw_usr_input); auto username = raw_usr_input;
    if (is_reserved(username)){
        send_message(std::format("\"{}\" is a reserved word. Please enter a different USER_NAME.", username));
        return;
    }
    new_entry.username = username;
    /* -=-=-=-   SECRET  -=-=-=- */
    send_message("Enter the associated SECRET: ");
    request_input(); read_raw_input(raw_usr_input);  auto secret = raw_usr_input;
    if (is_reserved(secret)){
        send_message(std::format("\"{}\" is a reserved word. Please enter a better secret to store.", secret));
        return;
    }
    new_entry.secret = secret;
    secrets[new_entry.name] = new_entry;
}

void edit_entry(std::string& raw_usr_input, Vault& secrets){
    /* -=-=-=-    NAME   -=-=-=- */
    send_message("Enter the NAME of the secret: ");
    request_input(); read_raw_input(raw_usr_input); auto name = raw_usr_input;
    if (is_reserved(name)){
        send_message(std::format("\"{}\" is a reserved word. Please enter a different NAME.", name));
        return;
    }
    Entry retrieved_entry = get_value_from(secrets, name, Entry{NULL_STR});
    if (retrieved_entry.name == NULL_STR){
        send_message(std::format("There is no entry with name {}.", name));
        return;
    }
    /* -=-=-=- USER_NAME -=-=-=- */
    send_message("Enter the new associated USER_NAME: ");
    request_input(); read_raw_input(raw_usr_input); auto username = raw_usr_input;
    if (is_reserved(username)){
        send_message(std::format("\"{}\" is a reserved word. Please enter a different USER_NAME.", username));
        return;
    }
    retrieved_entry.username = username;
    /* -=-=-=-   SECRET  -=-=-=- */
    send_message("Enter the new associated SECRET: ");
    request_input(); read_raw_input(raw_usr_input);  auto secret = raw_usr_input;
    if (is_reserved(secret)){
        send_message(std::format("\"{}\" is a reserved word. Please enter a better secret to store.", secret));
        return;
    }
    retrieved_entry.secret = secret;
    secrets[retrieved_entry.name] = retrieved_entry;
};

void delete_entry(std::string& raw_usr_input, Vault& secrets){
    send_message("Enter the NAME of the secret: ");
    request_input(); read_raw_input(raw_usr_input); auto name = raw_usr_input;
    Entry retrieved_entry = get_value_from(secrets, name, Entry{NULL_STR});
    if (retrieved_entry.name == NULL_STR){
        send_message(std::format("There is no entry with name {}.", name));
        return;
    }
    std::string confirmation = std::format("Delete {} secret", name);
    while (true){
        send_message(std::format("Type {} to confirm or \"STOP\" to cancel the deletion.", confirmation));
        request_input(); read_raw_input(raw_usr_input); auto usr_confirmation = raw_usr_input;
        if (usr_confirmation == confirmation){
            secrets.erase(name);
            return;
        } else if (usr_confirmation == "STOP") {
            return;
        }
    }
}

void display_entry(std::string& raw_usr_input, Vault& secrets){
    send_message("Enter the NAME of the secret: ");
    request_input(); read_raw_input(raw_usr_input); auto name = raw_usr_input;
    Entry retrieved_entry = get_value_from(secrets, name, Entry{NULL_STR});
    if (retrieved_entry.name == NULL_STR){
        send_message(std::format("There is no entry with name {}.", name));
        return;
    }
    send_message(
        std::format("NAME: {}\nUSER_NAME: {}\nSECRET: {}",
        retrieved_entry.name,
        retrieved_entry.username,
        retrieved_entry.secret)
    );
}

void list_entries(std::string& raw_usr_input, Vault& secrets){
    send_message("How many entries to list?");
    int n;
    while (n<=0){
        request_input(); read_raw_input(raw_usr_input);
        try{
            n = std::stoi(raw_usr_input);
        } catch (const std::invalid_argument&) {
            // No valid number.
        } catch (const std::out_of_range&) {
            // Number too large or too small.
        }
        send_message("Enter a valid positive integer.");
    }
    int c = 1;
    for (const auto& [name, entry] : secrets) {
        if (c >= n){return;}
        std::cout << name << ": " << entry.username << '\n';
        c++;
    }
}

void help(){}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

void start_mainloop(MainLoopState& MLS, Vault& secrets){
    send_message("Hello!");
    send_message("Ready when you are...");
    std::string raw_usr_input;
    while (MLS == RUNNING){
        request_input(); read_raw_input(raw_usr_input);
        auto raw_input_copy = raw_usr_input;
        Command usr_command = get_value_from(commands, raw_input_copy, Invalid);
        switch (usr_command){
        case AddEntry:
            add_entry(raw_input_copy, secrets);
            break;
        case EditEntry:
            std::cout << "Requested <EditEntry>\n";
            break;
        case DeleteEntry:
            std::cout << "Requested <DeleteEntry>\n";
            break;
        case Quit:
            std::cout << "Requested <Quit>\n";
            quit(MLS);
            break;
        case Help:
            std::cout << "Requested<Help>\n";
            break;
        default: // Invalid type
            send_message("Unknown command; Please input \"/help\" to see available commands");
        }
    }
}

int main(){
    Vault secrets = {}; // no offline persistence yet
    MainLoopState MLS = RUNNING;
    start_mainloop(MLS, secrets);
    return 0;
}
