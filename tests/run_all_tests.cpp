/**
 * @file run_all_tests.cpp
 * @brief Test executable entry point and process exit status.
 */
#include <cstdlib>
#include <iostream>
#include "test_vault.hpp"

/**
 * @brief Runs vault tests and prints an overall summary.
 * @return EXIT_SUCCESS if all checks pass; EXIT_FAILURE otherwise.
 */
int main() {
    if (vaultTests::testAll()){
        std::cout << "SUCCESS! ALL TESTS HAVE PASSED!\n";
        return EXIT_SUCCESS;
    } else{
        std::cout << "FAILURE! SOME TESTS HAVE FAILED!\n";
        return EXIT_FAILURE;
    }
}
