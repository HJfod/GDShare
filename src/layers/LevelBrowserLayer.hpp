#pragma once

#include "../offsets.hpp"
#include "GJSearchObject.hpp"

class LevelBrowserLayer : public cocos2d::CCLayer {
    protected:
        inline static bool (__thiscall* init)(LevelBrowserLayer*, gd::GJSearchObject*);
        static bool __fastcall initHook(LevelBrowserLayer*, uintptr_t, gd::GJSearchObject*);

        void onImport(cocos2d::CCObject*);
    
    public:
        static inline bool loadHook() {
            return MH_CreateHook(
                (PVOID)(gd::base + 0x15a040),
                as<LPVOID>(LevelBrowserLayer::initHook),
                as<LPVOID*>(&LevelBrowserLayer::init)
            ) == MH_OK;
        }

        static void importLevel(std::string const&);
        
        static LevelBrowserLayer* create(gd::GJSearchObject* pSearch) {
            return reinterpret_cast<LevelBrowserLayer*(__fastcall*)(
                gd::GJSearchObject*
            )>(
                gd::base + 0x159fa0
            )(
                pSearch
            );
        }
};
