#include "vault.hpp"

#include <algorithm>

std::string NULL_STR = "NULL";
std::vector<std::string> reserved_words{NULL_STR};

bool is_reserved(const std::string& s){
    return std::find(reserved_words.begin(), reserved_words.end(), s) != reserved_words.end();
}
