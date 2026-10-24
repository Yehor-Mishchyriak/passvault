// std library includes
#include <string>
#include <stdexcept>
// proj includes
#include "vault.hpp"

/*----------------------------------------------------*/
//                       HELPERS
/*----------------------------------------------------*/
bool
passvault::Vault::exists(const std::string& name) const{
    return this->secrets_.contains(name);
}
/*----------------------------------------------------*/

/*----------------------------------------------------*/
//                    CONSTRUCTORS            
/*----------------------------------------------------*/
passvault::Vault::Vault(
    const std::unordered_map<std::string, passvault::Secret>& secrets
) : secrets_(secrets) {}
/*----------------------------------------------------*/

/*----------------------------------------------------*/
//                     ADD METHODS
/*----------------------------------------------------*/
void
passvault::Vault::add(const passvault::Secret& secret){
    if (this->exists(secret.name)){
        throw std::invalid_argument("A secret with this name already exists");
    }
    this->secrets_.insert({secret.name, secret});
}
/*----------------------------------------------------*/

/*----------------------------------------------------*/
//                    REMOVE METHODS     
/*----------------------------------------------------*/
void
passvault::Vault::remove(const std::string& name){
    if (!(this->exists(name))){
        throw std::invalid_argument("A secret with this name does not exist");
    }
    this->secrets_.erase(name);
}
/*----------------------------------------------------*/

/*----------------------------------------------------*/
//                     GET METHODS
/*----------------------------------------------------*/
std::string
passvault::Vault::get(const std::string& name) const{
    return (this->secrets_.at(name)).value;
}
/*----------------------------------------------------*/

/*----------------------------------------------------*/
//                  READ-ONLY GETTERS
/*----------------------------------------------------*/
const std::unordered_map<std::string, passvault::Secret>&
passvault::Vault::secrets() const{
    return this->secrets_;
}
/*----------------------------------------------------*/
