#include "mod.hpp"
#include "layers/LevelBrowserLayer.hpp"
#include "layers/EditLevelLayer.hpp"
#include "layers/LevelInfoLayer.hpp"

bool mod::load() {
    if (MH_Initialize() != MH_OK) [[unlikely]]
        return false;
    
    LevelBrowserLayer::loadHook();
    EditLevelLayer::loadHook();
    LevelInfoLayer::loadHook();

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) [[unlikely]] {
        MH_Uninitialize();
        return false;
    }
    
    return true;
}

void mod::unload() {
    MH_DisableHook(MH_ALL_HOOKS);

    MH_Uninitialize();
}
