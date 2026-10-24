/**
 * @file test_vault.cpp
 * @brief Fixture definitions and tests of vault insertion, lookup, membership, and removal.
 */
// std library includes
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <format>
#include <cstddef>
#include <stdexcept>
// proj includes
#include "helpers.hpp"
#include "test_vault.hpp"
#include "../src/core/vault.hpp"

using helpers::vaultHelpers::VaultFixtureOptions;
using helpers::vaultHelpers::vaultFixture;
using helpers::testEquals;
using helpers::testThrows;
using passvault::Secret;

/*----------------------------------------------------*/
//                      FIXTURES
/*----------------------------------------------------*/
/// @brief Shared read-only fixture descriptions copied by individual tests.
namespace fixtures {

/// @brief Empty and populated inputs covering varied names, descriptions, and values.
const std::vector<VaultFixtureOptions> vaultFixtures{
    VaultFixtureOptions{},
    VaultFixtureOptions{
            .empty = false,
            .contents = {
                {"wesportal", {.name = "wesportal", .value = "12345"}},
                {"github", {.name = "github", .value = "abcdef"}}
            },
    },
    VaultFixtureOptions{
        .empty = false,
        .contents = {
            {"email", {.name = "email", .value = "mail-password"}}
        },
    },
    VaultFixtureOptions{
        .empty = false,
        .contents = {
            {"bank", {.name = "bank", .description = "Online banking", .value = "bank-password"}},
            {"card", {.name = "card", .description = "Card PIN", .value = "4826"}}
        },
    },
    VaultFixtureOptions{
        .empty = false,
        .contents = {
            {"empty-value", {.name = "empty-value", .value = ""}}
        },
    },
    VaultFixtureOptions{
        .empty = false,
        .contents = {
            {"spaces", {.name = "spaces", .description = "Value containing spaces", .value = "correct horse battery staple"}}
        },
    },
    VaultFixtureOptions{
        .empty = false,
        .contents = {
            {"symbols", {.name = "symbols", .value = "!@#$%^&*()_+-=[]{};:'\",.<>/?\\|"}}
        },
    },
    VaultFixtureOptions{
        .empty = false,
        .contents = {
            {"GitHub", {.name = "GitHub", .value = "uppercase"}},
            {"github", {.name = "github", .value = "lowercase"}}
        },
    },
    VaultFixtureOptions{
        .empty = false,
        .contents = {
            {"work email", {.name = "work email", .value = "work-password"}},
            {"personal email", {.name = "personal email", .value = "personal-password"}}
        },
    },
    VaultFixtureOptions{
        .empty = false,
        .contents = {
            {"multiline", {.name = "multiline", .description = "Value with a newline and tab", .value = "first line\nsecond\tline"}}
        },
    },
    VaultFixtureOptions{
        .empty = false,
        .contents = {
            {"long-value", {.name = "long-value", .value = std::string(1024, 'x')}}
        },
    },
    VaultFixtureOptions{
        .empty = false,
        .contents = {
            {"service-1", {.name = "service-1", .value = "shared-password"}},
            {"service-2", {.name = "service-2", .value = "shared-password"}},
            {"service-3", {.name = "service-3", .description = "Third service", .value = "different-password"}}
        },
    }
};

}; // namespace fixtures
/*----------------------------------------------------*/

/*----------------------------------------------------*/
//                      vaultTests
/*----------------------------------------------------*/
namespace vaultTests{

/**
 * @brief Checks insertion of a fresh secret into each fixture against the expected map.
 * @return True if all insertion checks pass.
 */
bool
testAdd(){
    std::vector<bool> results;
    for (std::size_t i = 0; i < fixtures::vaultFixtures.size(); ++i) {
        auto actualInitilizerKwargs = fixtures::vaultFixtures[i];
        auto expectedInitilizerKwargs = fixtures::vaultFixtures[i];
        expectedInitilizerKwargs.empty = false;
        expectedInitilizerKwargs.contents.insert({"FOO", Secret{.name="FOO", .value="BAR"}});
        auto actual = vaultFixture(actualInitilizerKwargs);
        auto expected = vaultFixture(expectedInitilizerKwargs);
        
        actual.add(Secret{.name="FOO", .value="BAR"});

        results.push_back(testEquals(
            std::format("Test Vault.add {} ", i+1),
            actual.secrets(),
            expected.secrets(),
            "SUCCESS",
            "FAIL",
            false
        ));
    }
    return std::ranges::all_of(results, [](bool passed) { return passed; });
}

/**
 * @brief Checks membership of existing keys and absence of a missing key in each fixture.
 * @return True if all membership checks pass.
 */
bool
testExists(){
    std::vector<bool> results;
    for (std::size_t i = 0; i < fixtures::vaultFixtures.size(); ++i) {
        auto actualInitilizerKwargs = fixtures::vaultFixtures[i];
        auto actual = vaultFixture(actualInitilizerKwargs);

        for (const auto& [name, secret] : actualInitilizerKwargs.contents) {
            results.push_back(testEquals(
                std::format("Test Vault.exists {}: {}", i+1, name),
                actual.exists(name),
                true,
                "SUCCESS",
                "FAIL",
                false
            ));
        }

        results.push_back(testEquals(
            std::format("Test Vault.exists {}: missing name", i+1),
            actual.exists("FOO"),
            false,
            "SUCCESS",
            "FAIL",
            false
        ));
    }
    return std::ranges::all_of(results, [](bool passed) { return passed; });
}

/**
 * @brief Checks removal against an expected map and rejection of a missing key.
 * @details Also checks that a rejected removal leaves the vault unchanged.
 * @return True if all removal and exception checks pass.
 */
bool
testRemove(){
    std::vector<bool> results;
    for (std::size_t i = 0; i < fixtures::vaultFixtures.size(); ++i) {
        for (const auto& [name, secret] : fixtures::vaultFixtures[i].contents) {
            auto actualInitilizerKwargs = fixtures::vaultFixtures[i];
            auto expectedInitilizerKwargs = fixtures::vaultFixtures[i];
            expectedInitilizerKwargs.contents.erase(name);
            auto actual = vaultFixture(actualInitilizerKwargs);
            auto expected = vaultFixture(expectedInitilizerKwargs);

            actual.remove(name);

            results.push_back(testEquals(
                std::format("Test Vault.remove {}: {}", i+1, name),
                actual.secrets(),
                expected.secrets(),
                "SUCCESS",
                "FAIL",
                false
            ));
        }

        auto actualInitilizerKwargs = fixtures::vaultFixtures[i];
        auto expectedInitilizerKwargs = fixtures::vaultFixtures[i];
        auto actual = vaultFixture(actualInitilizerKwargs);
        auto expected = vaultFixture(expectedInitilizerKwargs);

        results.push_back(testThrows<std::invalid_argument>(
            std::format("Test Vault.remove {}: missing name", i+1),
            &passvault::Vault::remove,
            "SUCCESS",
            "FAIL",
            false,
            actual,
            "FOO"
        ));
        results.push_back(testEquals(
            std::format("Test Vault.remove {}: unchanged after missing name", i+1),
            actual.secrets(),
            expected.secrets(),
            "SUCCESS",
            "FAIL",
            false
        ));
    }
    return std::ranges::all_of(results, [](bool passed) { return passed; });
}

/**
 * @brief Checks stored values and std::out_of_range for a missing key in each fixture.
 * @return True if all retrieval and exception checks pass.
 */
bool
testGet(){
    std::vector<bool> results;
    for (std::size_t i = 0; i < fixtures::vaultFixtures.size(); ++i) {
        auto actualInitilizerKwargs = fixtures::vaultFixtures[i];
        auto actual = vaultFixture(actualInitilizerKwargs);

        for (const auto& [name, secret] : actualInitilizerKwargs.contents) {
            results.push_back(testEquals(
                std::format("Test Vault.get {}: {}", i+1, name),
                actual.get(name),
                secret.value,
                "SUCCESS",
                "FAIL",
                false
            ));
        }

        results.push_back(testThrows<std::out_of_range>(
            std::format("Test Vault.get {}: missing name", i+1),
            &passvault::Vault::get,
            "SUCCESS",
            "FAIL",
            false,
            actual,
            "FOO"
        ));
    }
    return std::ranges::all_of(results, [](bool passed) { return passed; });
}

bool
testAll(){
    const std::vector<bool> results{
        testAdd(),
        testExists(),
        testGet(),
        testRemove()
    };
    return std::ranges::all_of(results, [](bool passed) { return passed; });
}
} // namespace vaultTests
/*----------------------------------------------------*/
