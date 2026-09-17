#pragma once

#include <string>

extern std::string NULL_STR;

struct Entry{
    std::string name = NULL_STR;
    std::string username = NULL_STR;
    std::string secret = NULL_STR;
};
