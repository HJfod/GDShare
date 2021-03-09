#pragma once

#ifndef INCLUDE_SUBMODULES
    #define INCLUDE_SUBMODULES
    #pragma warning( push, 0 )
    #include <cocos2d.h>
    #include <nfd.h>
    #include <MinHook.h>
    #pragma warning( pop )
#endif

#include "console.hpp"
//#define GDCONSOLE

#define __STR_CAT___(str1, str2) str1##str2
#define __STR_CAT__(str1, str2) __STR_CAT___(str1, str2)
#define PAD(size) char __STR_CAT__(pad, __LINE__)[size] = {};

const uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleA(0));

template<typename T>
static T getChild(cocos2d::CCNode* x, int i) {
    return static_cast<T>(x->getChildren()->objectAtIndex(i));
}

namespace GDShare::live {
    bool init();
    bool unload();
    void awaitUnload();
}

