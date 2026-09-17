#pragma once

#include "vault.hpp"

enum MainLoopState{
    RUNNING,
    ABORTED
};

void start_mainloop(MainLoopState& MLS, Vault& secrets);
