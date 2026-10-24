/**
 * @file vault.hpp
 * @brief Public types and operations for an in-memory vault.
 */
#pragma once

#include <string>
#include <unordered_map>

/// @brief In-memory secret records and vault operations.
namespace passvault {

/**
 * @brief A named secret with an optional description and a string value.
 */
struct Secret {
    std::string name; ///< Key used by Vault::add to store this secret.
    std::string description = ""; ///< Optional descriptive text.
    std::string value = ""; ///< Stored secret value; may be empty.
    /// @brief Compares name, description, and value for equality.
    /// @return True if every field matches the corresponding field in the other secret.
    bool operator==(const Secret&) const = default;
};

/**
 * @brief Owns an in-memory map of secrets keyed by name.
 * @details This class stores ordinary strings; it does not encrypt or persist them.
 */
class Vault {
public:
    /*----------------------------------------------------*/
    //                    CONSTRUCTORS            
    /*----------------------------------------------------*/
    /// @brief Constructs an empty vault.
    Vault() = default;
    /**
     * @brief Constructs a vault by copying the supplied map.
     * @param secrets Initial entries, copied without validating keys against Secret::name.
     */
    explicit Vault(
        const std::unordered_map<std::string, Secret>& secrets
    );
    /*----------------------------------------------------*/

    /*----------------------------------------------------*/
    //                     ADD METHODS
    /*----------------------------------------------------*/
    /**
     * @brief Copies a secret into the vault under its name.
     * @param secret Record to insert. Empty names and values are accepted.
     * @throws std::invalid_argument If the name already exists.
     */
    void
    add(const Secret& secret);
    /*----------------------------------------------------*/

    /*----------------------------------------------------*/
    //                    REMOVE METHODS     
    /*----------------------------------------------------*/
    /**
     * @brief Removes the entry with the given key.
     * @param name Key of the entry to remove.
     * @throws std::invalid_argument If the key does not exist.
     */
    void
    remove(const std::string& name);
    /*----------------------------------------------------*/

    /*----------------------------------------------------*/
    //                     GET METHODS
    /*----------------------------------------------------*/
    /**
     * @brief Retrieves the value stored under a key.
     * @param name Key to look up.
     * @return A copy of the stored value.
     * @throws std::out_of_range If the key does not exist.
     */
    std::string
    get(const std::string& name) const;
    /*----------------------------------------------------*/

    /*----------------------------------------------------*/
    //                       HELPERS
    /*----------------------------------------------------*/
    /**
     * @brief Checks whether a key is present without modifying the vault.
     * @param name Key to look up; matching is case-sensitive.
     * @return True if the key exists, including when its stored value is empty.
     */
    bool
    exists(const std::string& name) const;
    /*----------------------------------------------------*/

    /*----------------------------------------------------*/
    //                  READ-ONLY GETTERS
    /*----------------------------------------------------*/
    /**
     * @brief Provides read-only access to the owned map without copying it.
     * @return A const reference to the internal map.
     * @warning The reference must not outlive this vault. It observes subsequent mutations.
     */
    const std::unordered_map<std::string, Secret>& secrets() const;
    /*----------------------------------------------------*/

private:
    std::unordered_map<std::string, Secret> secrets_; ///< Records owned by this vault.
};

} // namespace passvault