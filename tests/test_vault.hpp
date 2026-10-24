/**
 * @file test_vault.hpp
 * @brief Declaration of the vault test suite entry point.
 */
#pragma once

/// @brief Fixture-based tests for the in-memory vault.
namespace vaultTests{
/**
 * @brief Runs all vault test groups, collecting every group result before aggregation.
 * @return True if every check passes; false if any check reports failure.
 * @note An uncaught exception interrupts execution rather than returning a result.
 */
bool
testAll();
}