#pragma once

#include <string>
#include <unordered_map>

namespace passvault {

struct Secret {
    std::string name;
    std::string description = "";
    std::string value = "";
    bool operator==(const Secret&) const = default;
};

class Vault {
public:
    /*----------------------------------------------------*/
    //                    CONSTRUCTORS            
    /*----------------------------------------------------*/
    Vault() = default;
    explicit Vault(
        const std::unordered_map<std::string, Secret>& secrets
    );
    /*----------------------------------------------------*/

    /*----------------------------------------------------*/
    //                     ADD METHODS
    /*----------------------------------------------------*/
    void
    add(const Secret& secret);
    /*----------------------------------------------------*/

    /*----------------------------------------------------*/
    //                    REMOVE METHODS     
    /*----------------------------------------------------*/
    void
    remove(const std::string& name);
    /*----------------------------------------------------*/

    /*----------------------------------------------------*/
    //                     GET METHODS
    /*----------------------------------------------------*/
    std::string
    get(const std::string& name) const;
    /*----------------------------------------------------*/

    /*----------------------------------------------------*/
    //                       HELPERS
    /*----------------------------------------------------*/
    bool
    exists(const std::string& name) const;
    /*----------------------------------------------------*/

    /*----------------------------------------------------*/
    //                  READ-ONLY GETTERS
    /*----------------------------------------------------*/
    const std::unordered_map<std::string, Secret>& secrets() const;
    /*----------------------------------------------------*/

private:
    std::unordered_map<std::string, Secret> secrets_;
};

} // namespace passvault