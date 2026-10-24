#pragma once

// std library includes
#include <string>
#include <exception>
#include <iostream>
#include <functional> // std::invoke
#include <utility>    // std::forward
// proj includes
#include "../src/core/vault.hpp"

namespace helpers {

/*----------------------------------------------------*/
//                    VAULT HELPERS
/*----------------------------------------------------*/
namespace vaultHelpers {

struct VaultFixtureOptions{
    bool empty = true;
    std::unordered_map<std::string, passvault::Secret> contents;
};

passvault::Vault
vaultFixture(
    const VaultFixtureOptions& options
);

} // namespace vaultHelpers
/*----------------------------------------------------*/

/*----------------------------------------------------*/
//                      GENERICS
/*----------------------------------------------------*/
template <typename Actual, typename Expected>
bool
testEquals(
    const std::string& testName,
    const Actual& actual,
    const Expected& expected,
    const std::string& successMsg,
    const std::string& failMsg
){

    bool passed = (actual == expected);

    std::cout
        << testName << ": "
        << (passed ? successMsg : failMsg) << "\n";

    return passed;
}

// call as testThrows<Exception>
template <typename Exception, typename F, typename... Args>
bool testThrows(
    const std::string& testName,
    F&& function,
    const std::string& successMsg,
    const std::string& failMsg,
    Args&&... args
) {
    std::cout << testName << ": ";

    try {
        std::invoke( // also handles pointers to member functions
            std::forward<F>(function),
            std::forward<Args>(args)...
        ); // ^ note the use of std::forward to preserve whether the arg was l/rvalue- passed
    } catch (const Exception&) {
        std::cout << successMsg << '\n';
        return true;
    } catch (...) {
        std::cout << failMsg << " (unexpected exception)\n";
        return false;
    }

    std::cout << failMsg << " (no exception)\n";
    return false;
}

} // namespace helpers
/*----------------------------------------------------*/
