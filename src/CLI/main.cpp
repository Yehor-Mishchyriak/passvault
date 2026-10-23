// std library includes
#include <iostream>
#include <string>
#include <vector>
// proj includes
#include <core/vault.hpp>


int main() {
    passvault::Vault V;
    V.add(passvault::Secret{.name="wesportal", .value="12345"});
    std::cout << V.get("wesportal") << "\n";
    return 0;
}
