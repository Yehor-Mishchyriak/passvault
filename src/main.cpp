#include "cli.hpp"
#include "vault.hpp"

int main(){
    Vault secrets = {}; // no offline persistence yet
    MainLoopState MLS = RUNNING;
    start_mainloop(MLS, secrets);
    return 0;
}
