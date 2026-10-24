/**
 * @file helpers.hpp
 * @brief Fixture options and reusable equality and exception test helpers.
 */
#pragma once

// std library includes
#include <string>
#include <exception>
#include <iostream>
#include <functional> // std::invoke
#include <utility>    // std::forward
// proj includes
#include "../src/core/vault.hpp"

/// @brief Fixture construction and reusable test checks.
namespace helpers {

/*----------------------------------------------------*/
//                    VAULT HELPERS
/*----------------------------------------------------*/
/// @brief Helpers for constructing independent vaults for tests.
namespace vaultHelpers {

/**
 * @brief Selects empty construction or construction from a supplied map.
 */
struct VaultFixtureOptions{
    bool empty = true; ///< When true, contents is ignored and the vault is empty.
    std::unordered_map<std::string, passvault::Secret> contents; ///< Entries copied when empty is false.
};

/**
 * @brief Creates a vault from fixture options.
 * @param options Construction settings; contents is ignored when empty is true.
 * @return An independent vault, empty or containing a copy of options.contents.
 */
passvault::Vault
vaultFixture(
    const VaultFixtureOptions& options
);

} // namespace vaultHelpers
/*----------------------------------------------------*/

/*----------------------------------------------------*/
//                      GENERICS
/*----------------------------------------------------*/
/**
 * @brief Compares two values and reports failures, optionally reporting success.
 * @tparam Actual Type of the actual value; equality with Expected must be valid.
 * @tparam Expected Type of the expected value.
 * @param testName Label printed with a reported result.
 * @param actual Observed value.
 * @param expected Required value.
 * @param successMsg Text printed for a passing check when reportSuccess is true.
 * @param failMsg Text printed for a failing check.
 * @param reportSuccess Whether to print passing checks; failures are always printed.
 * @return The boolean result of actual == expected.
 * @note Exceptions from equality comparison are not caught.
 */
template <typename Actual, typename Expected>
bool
testEquals(
    const std::string& testName,
    const Actual& actual,
    const Expected& expected,
    const std::string& successMsg,
    const std::string& failMsg,
    const bool& reportSuccess
){

    bool passed = (actual == expected);
    if (!passed){
        std::cout << testName << ": " << failMsg << "\n";
    } else if (reportSuccess){
        std::cout << testName << ": " << successMsg << "\n";
    }

    return passed;
}

/**
 * @brief Invokes a callable and checks that it throws the requested exception type.
 * @tparam Exception Exception type to catch; specify explicitly as testThrows<Exception>.
 * @tparam F Callable type, deduced from function.
 * @tparam Args Argument types, deduced and forwarded to the callable.
 * @param testName Label printed with a reported result.
 * @param function Callable to invoke, including a pointer to a member function.
 * @param successMsg Text printed when the expected exception is caught and reporting is enabled.
 * @param failMsg Text printed if no exception or an unexpected exception is observed.
 * @param reportSuccess Whether to print passing checks; failures are always printed.
 * @param args Arguments to invoke with; for a member function, supply its object first.
 * @return True if an exception matching const Exception& is caught, including derived types.
 * @details Returns false for no exception or an unexpected exception; unexpected exceptions
 * are caught rather than propagated. The callable may modify arguments passed by reference.
 */

template <typename Exception, typename F, typename... Args>
bool testThrows(
    const std::string& testName,
    F&& function,
    const std::string& successMsg,
    const std::string& failMsg,
    const bool& reportSuccess,
    Args&&... args
) {
    try {
        std::invoke( // also handles pointers to member functions
            std::forward<F>(function),
            std::forward<Args>(args)...
        ); // ^ note the use of std::forward to preserve whether the arg was l/rvalue- passed
    } catch (const Exception&) {
        if (reportSuccess){
            std::cout << testName << ": " << successMsg << '\n';
        }
        return true;
    } catch (...) {
        std::cout << testName << ": " << failMsg << " (unexpected exception)\n";
        return false;
    }

    std::cout << testName << ": " << failMsg << " (no exception)\n";
    return false;
}

} // namespace helpers
/*----------------------------------------------------*/
