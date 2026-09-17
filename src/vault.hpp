#pragma once

#include "entry.hpp"

#include <string>
#include <unordered_map>
#include <vector>

using Vault = std::unordered_map<std::string, Entry>;

extern std::vector<std::string> reserved_words;

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

bool is_reserved(const std::string& s);
