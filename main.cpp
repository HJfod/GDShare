// generated through GDMake https://github.com/HJfod/GDMake

// include GDMake & submodules
#include <GDMake.h>
#include "src/layers/LevelBrowserLayer.hpp"
#include "src/layers/EditLevelLayer.hpp"
#include "src/layers/LevelInfoLayer.hpp"

GDMAKE_MAIN {
    LevelBrowserLayer::loadHook();
    EditLevelLayer::loadHook();
    LevelInfoLayer::loadHook();
}

GDMAKE_UNLOAD {}
