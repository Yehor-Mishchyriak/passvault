// std library includes
#include <string>
#include <unordered_map>
#include <iostream>
// proj includes
#include "helpers.hpp"

/*----------------------------------------------------*/
//                    VAULT HELPERS
/*----------------------------------------------------*/

namespace helpers::vaultHelpers{

passvault::Vault
vaultFixture(
    const VaultFixtureOptions& options
){
    if (options.empty){
        return passvault::Vault();
    }
    return passvault::Vault(options.contents);
}

}
/*----------------------------------------------------*/